#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TApplication.h"
#include "TChain.h"
#include "TTreeFormula.h"
#include "TCanvas.h"

#include "TLine.h"

#include <iostream>
#include <fstream>

//---- from Event.hpp
#define MAX_ADC_CHANNELS 200
#define MAX_DIGI_SAMPLES 100000
#define MAX_TDC_CHANNELS 200
#define MAX_SCALER_WORDS 16
#define MAX_PATTERNS 16
#define MAX_PATTERNS_SHODO 16
#define SMALL_HODO_X_NFIBERS 8
#define SMALL_HODO_Y_NFIBERS 8
#define MAX_TRIG_WORDS 32
#define MAX_RO 10

//---- from TBtree Shashlik ----
#include "include/TBEvent.h"
#include "include/HodoCluster.h"
#include "include/TBRecHit.h"
#include "include/Mapper.h"



#define hodoX1 0
#define hodoY1 1
#define hodoX2 2
#define hodoY2 3


//---- draw shashlik matrix

void DrawShashlikModule(TPad* cc){
 
 cc->cd();
 for (int i=0; i<5; i++) {
//   TString name = Form ("vert_%d",i);
//   TLine* vert = new TLine (name.Data(),i*14-28,-28,i*14-28,28);
  TLine* vert = new TLine (i*14-28,-28,i*14-28,28);
  vert->SetLineColor(kRed);
  vert->DrawClone("same");
 }

 for (int i=0; i<5; i++) {
//   TString name = Form ("oriz_%d",i);
//   TLine* oriz = new TLine (name.Data(),-28,i*14-28,28,i*14-28);
  TLine* oriz = new TLine (-28,i*14-28,28,i*14-28);
  oriz->SetLineColor(kRed);
  oriz->DrawClone("same");
 }
 
}



//---- boost
// #include "boost/program_options.hpp"
// #include "boost/program_options/options_backescription.hpp"

// namespace po = boost::program_options;

#include <sstream> 


//---- transform map into vectors
void TransformFibers(std::map<std::pair<int,int>, int > fibers, std::vector <int>& fibers_X1, std::vector <int>& fibers_X2, std::vector <int>& fibers_Y1, std::vector <int>& fibers_Y2){
  
 std::pair<int,int> fibers_mappairY1;
 fibers_mappairY1.first  = hodoY1;
 std::pair<int,int> fibers_mappairY2;
 fibers_mappairY2.first  = hodoY2;
 
 //---- Y direction is inverted !?!?
 for(int iBinY=63;iBinY>=0;iBinY--){
//   for(int iBinY=0;iBinY<64;iBinY++){
  fibers_mappairY1.second = iBinY;
  fibers_mappairY2.second = iBinY;
  fibers_Y1.push_back( fibers[fibers_mappairY1] );
  fibers_Y2.push_back( fibers[fibers_mappairY2] );
 }

 std::pair<int,int> fibers_mappairX1;
 fibers_mappairX1.first  = hodoX1;
 std::pair<int,int> fibers_mappairX2;
 fibers_mappairX2.first  = hodoX2;
 for(int iBinX=0;iBinX<64;iBinX++){
  fibers_mappairX1.second = iBinX;
  fibers_mappairX2.second = iBinX;
  fibers_X1.push_back( fibers[fibers_mappairX1] );
  fibers_X2.push_back( fibers[fibers_mappairX2] );
 }
 
}



//---- Hodoscope clusters
std::vector<HodoCluster*> getHodoClusters( std::vector<int> hodo) {
 float fibreWidth = 0.5;
 int nClusterMax = 10;
 float Cut = 0;
 
 std::vector<HodoCluster*> clusters;
 HodoCluster* currentCluster = new HodoCluster( hodo.size(), fibreWidth );
 for ( unsigned i=0; i<hodo.size(); ++i ) {
  if ( hodo[i] > Cut) { // hit
   if ( currentCluster->getSize() < nClusterMax ) {
    currentCluster->addFibre( i );
   } else {
    clusters.push_back( currentCluster ); // store old one
    currentCluster = new HodoCluster( hodo.size(), fibreWidth ); // create a new one
    currentCluster->addFibre( i ); // get that fibre!
   }
  } else { // as soon as you find a hole
   if ( currentCluster->getSize() > 0 ) {
    clusters.push_back( currentCluster ); // store old one
    currentCluster = new HodoCluster( hodo.size(), fibreWidth ); // create a new one
   }
  }
 } // for fibres
 if ( currentCluster->getSize()>0 )
  clusters.push_back( currentCluster ); // store last cluster
  return clusters;
}


//---- Reconstruct Hodoscope clusters
void doHodoReconstruction( std::vector<int> input_values, std::vector<int>& nFibres, std::vector<float>& cluster_position, float table) {
 std::vector<HodoCluster*> clusters = getHodoClusters( input_values );
 for( unsigned i=0; i<clusters.size(); ++i ) {
  nFibres.push_back( clusters[i]->getSize() );
  cluster_position.push_back( clusters[i]->getPosition() - table );
 }
}




//---- distance definition in calorimeter position
float DR (float x1, float x2, float y1, float y2) {
 return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}





//---- Reconstruct Calorimeter clusters
void doCalorimeterReconstruction( Mapper* mapper,  std::vector<TBRecHit>* rechits, std::vector<float>& caloCluster_position_X, std::vector<float>& caloCluster_position_Y, std::vector<float>& caloCluster_Energy, int face, float maxDR, int fiberLevel = 0) {
  
 //----      adc              X      Y
 std::map < float, std::pair<float, float> > map_of_calo_clusters;
 float max;
 int channelID;
 for (Int_t j = 0; j < rechits->size(); j++){
  TBRecHit &hit=rechits->at(j);
//   ped = hit.Pedestal();
//   sig = hit.NoiseRMS();
  max = hit.AMax();
//   maxTime = hit.TRise();
  channelID = hit.GetChannelID();
  
  if (max<0.1) continue; //---- very low threshold, just to remove noisy noise
  
  int moduleID,fiberID;
  mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs
  
  double x,y;
  if (fiberLevel == 0) mapper->ModuleXY(moduleID,x,y);
  else                 mapper->FiberXY(fiberID, x, y);
  
  //---- only the wanted face (front [1] or back [-1])
  if (moduleID<0 && face>0) {
   continue;
  }
  if (moduleID>0 && face<0) {
   continue;
  }
  
  std::pair<float, float> xy_pair;
  xy_pair.first = x;
  xy_pair.second = y;
  
  map_of_calo_clusters[-max] = xy_pair;
 }     
 
 //---- dump the map into the std::vector
//  for( std::map < float, std::pair<float, float> >::iterator ii=map_of_calo_clusters.begin(); ii!=map_of_calo_clusters.end(); ii++) {
//   caloCluster_Energy.push_back( ii->first );
//   caloCluster_position_X.push_back( ii->second.first );
//   caloCluster_position_Y.push_back( ii->second.second );
//  }

 
 
 //---- do clustering ----
//  int num_clusters = 0;
 float x_cluster_logE = 0;
 float y_cluster_logE = 0;
 float weight_cluster = 0;
 float energy_cluster = 0;
 
 float x_cluster_max = -1000;
 float y_cluster_max = -1000;
 
 //---- first calculate cluster energy
 for( std::map < float, std::pair<float, float> >::iterator ii=map_of_calo_clusters.begin(); ii!=map_of_calo_clusters.end(); ii++) {
//   std::cout << " energy = " << - ii->first << std::endl;
  if (x_cluster_max == -1000) {
   x_cluster_max = (ii->second.first); //---- seed position X
   y_cluster_max = (ii->second.second); //---- seed position X   
   float energy_temp = - ii->first;
   energy_cluster = energy_cluster + energy_temp;
  }
  else {
   if (DR(ii->second.first, x_cluster_max, ii->second.second, y_cluster_max) < maxDR) {
    float energy_temp = - ii->first;
    energy_cluster = energy_cluster + energy_temp;
    //   num_clusters++;
   }
  }
 }
 
 //---- then calculate position 
//  std::cout<< " new cluster " << std::endl;
//  float w0 = 3.8;
 float w0 = 5.0;
 if (energy_cluster > 0) {
  for( std::map < float, std::pair<float, float> >::iterator ii=map_of_calo_clusters.begin(); ii!=map_of_calo_clusters.end(); ii++) {
   //   std::cout << " energy = " << - ii->first << std::endl;
   if (DR(ii->second.first, x_cluster_max, ii->second.second, y_cluster_max) < maxDR) {
    float energy_temp = - ii->first;
    float wi = (w0 + log(energy_temp/energy_cluster));
    if (wi > 0) {
     x_cluster_logE = x_cluster_logE + (ii->second.first)  * wi ;
     y_cluster_logE = y_cluster_logE + (ii->second.second) * wi ;
     weight_cluster = weight_cluster + wi;
    }
   }
  }
 }

 float x_cluster_final = 0;
 float y_cluster_final = 0;
 
 if (weight_cluster != 0) {
  x_cluster_final = x_cluster_logE / weight_cluster;
  y_cluster_final = y_cluster_logE / weight_cluster;

  caloCluster_Energy.push_back( energy_cluster );
  caloCluster_position_X.push_back( x_cluster_final );
  caloCluster_position_Y.push_back( y_cluster_final );
 }
 
//  std::cout << " num_clusters = " << num_clusters << std::endl;
} 
  

int main(int argc, char**argv){
 
 std::string input_file;
 int maxEvents = -1;
 int doFiber = 0;
 float table_x = 200; //---- mm
 float table_y = 350; //---- mm
 //---- configuration
 
 int c;
 while ((c = getopt (argc, argv, "i:m:f:x:y:")) != -1)
  switch (c)
  {
   case 'i': //---- input
    input_file = string(optarg);
    break;
   case 'm':
    maxEvents =  atoi(optarg);
    break;
   case 'f':
    doFiber =  atoi(optarg);
    break;
   case 'x':
    table_x =  atof(optarg);
    break;
   case 'y':
    table_y =  atof(optarg);
    break;
    
   case '?':
    if (optopt == 'i' || optopt == 'm' || optopt == 'f' || optopt == 'x' || optopt == 'y')
     fprintf (stderr, "Option -%c requires an argument.\n", optopt);
    else if (isprint (optopt))
     fprintf (stderr, "Unknown option `-%c'.\n", optopt);
    else
     fprintf (stderr,
              "Unknown option character `\\x%x'.\n",
              optopt);
     return 1;
   default:
    exit (-1);
  }
  
  std::cout << " Table: " << std::endl;
  std::cout << "   x = " << table_x << " mm " << std::endl;
  std::cout << "   y = " << table_y << " mm " << std::endl;
  
  
  //---- get vector of files
  
  std::vector<std::string> input_files_vector;
  std::stringstream ss(input_file);
  
  std::string token_string;
  while(std::getline(ss, token_string, ',')) {
   std::cout << token_string << '\n';
   input_files_vector.push_back(token_string);
  }
  
  std::cout << " input files:" << std::endl;
  for (int i=0; i<input_files_vector.size(); i++) {
   std::cout << " beam: " << input_files_vector.at(i) << std::endl;
  }
  
  //---- configuration (end)
  
  
  //---- read file
  
  TChain* H4tree = new TChain("t1041");
  for (unsigned int i=0; i<input_files_vector.size(); i++) {
   H4tree->Add(input_files_vector.at(i).c_str());
  }
  
  
  //---- read file
  int nEntries = H4tree->GetEntries(); 
  std::cout << " nEntries = " << nEntries << std::endl;
  if (maxEvents != -1) nEntries = maxEvents>nEntries ? nEntries : maxEvents ;
  std::cout << " new nEntries = " << nEntries << std::endl;
  
  
  TBSpill* tbspill = new TBSpill();
  TBEvent* tbevent = new TBEvent();
  
  TBranch *branch_event = H4tree->GetBranch("tbevent");
  branch_event->SetAddress(&tbevent);
  TBranch *branch_spill = H4tree->GetBranch("tbspill");
  branch_spill->SetAddress(&tbspill);
  
  TApplication* gMyRootApp = new TApplication("My ROOT Application", &argc, argv);
  
  TCanvas* cc_X = new TCanvas ("cc_X","",800,600);
  TCanvas* cc_Y = new TCanvas ("cc_Y","",800,600);
  
  TH2F *hHS_HS1_Cal_front_X  = new TH2F("hHS_HS1_Cal_front_X", "Hodoscope 1 vs Cal front X", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2_Cal_front_X  = new TH2F("hHS_HS2_Cal_front_X", "Hodoscope 2 vs Cal front X", 64, -32, 32, 64, -32, 32);  
  TH2F *hHS_HS1_Cal_front_Y  = new TH2F("hHS_HS1_Cal_front_Y", "Hodoscope 1 vs Cal front Y", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2_Cal_front_Y  = new TH2F("hHS_HS2_Cal_front_Y", "Hodoscope 2 vs Cal front Y", 64, -32, 32, 64, -32, 32);
  
  TH2F *hHS_HS1_Cal_back_X  = new TH2F("hHS_HS1_Cal_back_X", "Hodoscope 1 vs Cal back X", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2_Cal_back_X  = new TH2F("hHS_HS2_Cal_back_X", "Hodoscope 2 vs Cal back X", 64, -32, 32, 64, -32, 32);  
  TH2F *hHS_HS1_Cal_back_Y  = new TH2F("hHS_HS1_Cal_back_Y", "Hodoscope 1 vs Cal back Y", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2_Cal_back_Y  = new TH2F("hHS_HS2_Cal_back_Y", "Hodoscope 2 vs Cal back Y", 64, -32, 32, 64, -32, 32);
  
  TCanvas* cc_DX = new TCanvas ("cc_DX","",800,400);
  
  TH2F *X_hHS_HS1_Cal_front  = new TH2F("X_hHS_HS1_Cal_front", "X Hodoscope 1 vs Cal front ", 64, -32, 32, nEntries, 0, nEntries);
  TH2F *X_hHS_HS2_Cal_front  = new TH2F("X_hHS_HS2_Cal_front", "X Hodoscope 2 vs Cal front ", 64, -32, 32, nEntries, 0, nEntries);
  TH2F *X_hHS_HS1_Cal_back   = new TH2F("X_hHS_HS1_Cal_back",  "X Hodoscope 1 vs Cal back " , 64, -32, 32, nEntries, 0, nEntries);
  TH2F *X_hHS_HS2_Cal_back   = new TH2F("X_hHS_HS2_Cal_back",  "X Hodoscope 2 vs Cal back " , 64, -32, 32, nEntries, 0, nEntries);

  
  TH1F *X_h1_HS1_Cal_front  = new TH1F("X_h1_HS1_Cal_front", "X Hodoscope 1 vs Cal front ", 256, -32, 32);
  TH1F *X_h1_HS2_Cal_front  = new TH1F("X_h1_HS2_Cal_front", "X Hodoscope 2 vs Cal front ", 256, -32, 32);
  TH1F *X_h1_HS1_Cal_back   = new TH1F("X_h1_HS1_Cal_back",  "X Hodoscope 1 vs Cal back " , 256, -32, 32);
  TH1F *X_h1_HS2_Cal_back   = new TH1F("X_h1_HS2_Cal_back",  "X Hodoscope 2 vs Cal back " , 256, -32, 32);
  
  
  TCanvas* cc_DY = new TCanvas ("cc_DY","",800,400);
  
  TH2F *Y_hHS_HS1_Cal_front  = new TH2F("Y_hHS_HS1_Cal_front", "Y Hodoscope 1 vs Cal front ", 64, -32, 32, nEntries, 0, nEntries);
  TH2F *Y_hHS_HS2_Cal_front  = new TH2F("Y_hHS_HS2_Cal_front", "Y Hodoscope 2 vs Cal front ", 64, -32, 32, nEntries, 0, nEntries);
  TH2F *Y_hHS_HS1_Cal_back   = new TH2F("Y_hHS_HS1_Cal_back",  "Y Hodoscope 1 vs Cal back " , 64, -32, 32, nEntries, 0, nEntries);
  TH2F *Y_hHS_HS2_Cal_back   = new TH2F("Y_hHS_HS2_Cal_back",  "Y Hodoscope 2 vs Cal back " , 64, -32, 32, nEntries, 0, nEntries);
  
  
  TH1F *Y_h1_HS1_Cal_front  = new TH1F("Y_h1_HS1_Cal_front", "Y Hodoscope 1 vs Cal front ", 256, -32, 32);
  TH1F *Y_h1_HS2_Cal_front  = new TH1F("Y_h1_HS2_Cal_front", "Y Hodoscope 2 vs Cal front ", 256, -32, 32);
  TH1F *Y_h1_HS1_Cal_back   = new TH1F("Y_h1_HS1_Cal_back",  "Y Hodoscope 1 vs Cal back " , 256, -32, 32);
  TH1F *Y_h1_HS2_Cal_back   = new TH1F("Y_h1_HS2_Cal_back",  "Y Hodoscope 2 vs Cal back " , 256, -32, 32);
  
  
  
  TCanvas* cc_hodo = new TCanvas ("cc_hodo","",800,800);
  
  TH2F *hHS_HS2_HS1_X  = new TH2F("hHS_HS2_HS1_X", "Hodoscope 2 vs Hodoscope 1 X", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2_HS1_Y  = new TH2F("hHS_HS2_HS1_Y", "Hodoscope 2 vs Hodoscope 1 Y", 64, -32, 32, 64, -32, 32);
    
  TH2F *hHS_HS1  = new TH2F("hHS_HS1", "Hodoscope 1", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2  = new TH2F("hHS_HS2", "Hodoscope 2", 64, -32, 32, 64, -32, 32);
  
  TCanvas* cc_Cal = new TCanvas ("cc_Cal","",800,800);
  
  TH2F *hHS_Cal_front;
  TH2F *hHS_Cal_back;

  TH2F *hHS_Cal_front_module;
  TH2F *hHS_Cal_back_module;
  
  if (doFiber) {
   hHS_Cal_front  = new TH2F("hHS_Cal_front", "Cal front ", 48, -28, 28, 48, -28, 28);
   hHS_Cal_back   = new TH2F("hHS_Cal_back",  "Cal back " , 48, -28, 28, 48, -28, 28);
   
   hHS_Cal_front_module  = new TH2F("hHS_Cal_front_module", "Cal front ", 24, -28, 28, 24, -28, 28);
   hHS_Cal_back_module   = new TH2F("hHS_Cal_back_module",  "Cal back " , 24, -28, 28, 24, -28, 28);
  }
  else {
   hHS_Cal_front  = new TH2F("hHS_Cal_front", "Cal front ", 72, -28, 28, 72, -28, 28);
   hHS_Cal_back   = new TH2F("hHS_Cal_back",  "Cal back " , 72, -28, 28, 72, -28, 28);
   
   hHS_Cal_front_module  = new TH2F("hHS_Cal_front_module", "Cal front ", 12, -28, 28, 12, -28, 28);
   hHS_Cal_back_module   = new TH2F("hHS_Cal_back_module",  "Cal back " , 12, -28, 28, 12, -28, 28);
  }
  
  
  
  
  bool haverechits = false;
  std::vector<TBRecHit> *rechits=0;
  if(H4tree->GetListOfBranches()->FindObject("tbrechits")) {
   std::cout << " found rechits " << std::endl;
   H4tree->SetBranchAddress("tbrechits",&rechits);
   haverechits = true;
  }
  
  Mapper *mapper = Mapper::Instance();
  
  for (int i=0; i<nEntries; i++) {
   
   if ((i%1000)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree->GetEntry(i);
   
   //---- calorimeter data
   if (i==0) mapper->SetEpoch(tbevent->GetTimeStamp());

   std::vector<float> caloCluster_position_X_front;
   std::vector<float> caloCluster_position_Y_front;
   std::vector<float> caloCluster_Energy_front;
   
   doCalorimeterReconstruction( mapper, rechits, caloCluster_position_X_front, caloCluster_position_Y_front, caloCluster_Energy_front, 1, 30, doFiber);    
    
   std::vector<float> caloCluster_position_X_back;
   std::vector<float> caloCluster_position_Y_back;
   std::vector<float> caloCluster_Energy_back;
   
   doCalorimeterReconstruction( mapper, rechits, caloCluster_position_X_back, caloCluster_position_Y_back, caloCluster_Energy_back, -1, 30, doFiber);
    
 
   
   
   //---- modular level DR = 5 mm
   std::vector<float> caloCluster_position_X_front_module;
   std::vector<float> caloCluster_position_Y_front_module;
   std::vector<float> caloCluster_Energy_front_module;
   
   doCalorimeterReconstruction( mapper, rechits, caloCluster_position_X_front_module, caloCluster_position_Y_front_module, caloCluster_Energy_front_module, 1, 5, doFiber);    
   
   std::vector<float> caloCluster_position_X_back_module;
   std::vector<float> caloCluster_position_Y_back_module;
   std::vector<float> caloCluster_Energy_back_module;
   
   doCalorimeterReconstruction( mapper, rechits, caloCluster_position_X_back_module, caloCluster_position_Y_back_module, caloCluster_Energy_back_module, -1, 5, doFiber);
   
   
   //---- hodoscope data
   Hodoscope hsch = tbevent->GetHSChan();
   //   hsch.Dump();
   std::map<std::pair<int,int>, int > fibers = hsch.GetFibers();
   
   std::vector <int> fibers_X1;
   std::vector <int> fibers_X2;
   std::vector <int> fibers_Y1;
   std::vector <int> fibers_Y2;
   
   TransformFibers(fibers, fibers_X1, fibers_X2, fibers_Y1, fibers_Y2);
     
   
   std::vector <int> n_fibers_X1;
   std::vector <int> n_fibers_X2;
   std::vector <int> n_fibers_Y1;
   std::vector <int> n_fibers_Y2;
   
   std::vector <float> pos_fibers_X1; //---- units is mm
   std::vector <float> pos_fibers_X2; //---- units is mm
   std::vector <float> pos_fibers_Y1; //---- units is mm
   std::vector <float> pos_fibers_Y2; //---- units is mm
   
   
   //----                                                  table position in mm
   doHodoReconstruction( fibers_X1, n_fibers_X1, pos_fibers_X1, (table_x - 200)); //---- change of coordinate system using numbers from googledoc
   doHodoReconstruction( fibers_X2, n_fibers_X2, pos_fibers_X2, (table_x - 200)); //---- change of coordinate system using numbers from googledoc
   doHodoReconstruction( fibers_Y1, n_fibers_Y1, pos_fibers_Y1, (table_y - 350)); //---- change of coordinate system using numbers from googledoc
   doHodoReconstruction( fibers_Y2, n_fibers_Y2, pos_fibers_Y2, (table_y - 350)); //---- change of coordinate system using numbers from googledoc
   
   //---- just hodoscope information
   for (int iCluster1 = 0; iCluster1 < pos_fibers_X1.size(); iCluster1++) {
    for (int iCluster2 = 0; iCluster2 < pos_fibers_X2.size(); iCluster2++) {
     hHS_HS2_HS1_X->Fill(pos_fibers_X1.at(iCluster1),pos_fibers_X2.at(iCluster2));
//      std::cout << " pos_fibers_X:: " << pos_fibers_X1.at(iCluster1) << " :: " << pos_fibers_X2.at(iCluster2) << std::endl;
    }
   }
   
   for (int iCluster1 = 0; iCluster1 < pos_fibers_Y1.size(); iCluster1++) {
    for (int iCluster2 = 0; iCluster2 < pos_fibers_Y2.size(); iCluster2++) {
     hHS_HS2_HS1_Y->Fill(pos_fibers_Y1.at(iCluster1),pos_fibers_Y2.at(iCluster2));
    }
   }
   
   for (int iCluster1 = 0; iCluster1 < pos_fibers_X1.size(); iCluster1++) {
    for (int iCluster2 = 0; iCluster2 < pos_fibers_Y1.size(); iCluster2++) {
     hHS_HS1->Fill(pos_fibers_X1.at(iCluster1),pos_fibers_Y1.at(iCluster2));
    }
   }

   for (int iCluster1 = 0; iCluster1 < pos_fibers_X2.size(); iCluster1++) {
    for (int iCluster2 = 0; iCluster2 < pos_fibers_Y2.size(); iCluster2++) {
     hHS_HS2->Fill(pos_fibers_X2.at(iCluster1),pos_fibers_Y2.at(iCluster2));
    }
   }
   
   
   
   
   //---- now merge and compare
   if (pos_fibers_X1.size() > 1) {
    
    //---- X
    for (int iCluster = 0; iCluster < pos_fibers_X1.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
//       std::cout << " caloCluster_position_X_front.at(" << iCalo << "), pos_fibers_X1.at(" << iCluster << ")  = " << caloCluster_position_X_front.at(iCalo) << "," <<  pos_fibers_X1.at(iCluster) << std::endl;
      hHS_HS1_Cal_front_X->Fill(caloCluster_position_X_front.at(iCalo), pos_fibers_X1.at(iCluster));
      X_hHS_HS1_Cal_front->Fill(caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster), i);
      X_h1_HS1_Cal_front->Fill(caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
      hHS_HS1_Cal_back_X->Fill(caloCluster_position_X_back.at(iCalo), pos_fibers_X1.at(iCluster));
      X_hHS_HS1_Cal_back->Fill(caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster), i);
      X_h1_HS1_Cal_back->Fill(caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster));
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_X2.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
      hHS_HS2_Cal_front_X->Fill(caloCluster_position_X_front.at(iCalo), pos_fibers_X2.at(iCluster));
      X_hHS_HS2_Cal_front->Fill(caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster), i);
      X_h1_HS2_Cal_front->Fill(caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
      hHS_HS2_Cal_back_X->Fill(caloCluster_position_X_back.at(iCalo), pos_fibers_X2.at(iCluster));
      X_hHS_HS2_Cal_back->Fill(caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster), i);
      X_h1_HS2_Cal_back->Fill(caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster));
     }
    }
    
    //---- Y
    for (int iCluster = 0; iCluster < pos_fibers_Y1.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_Y_front.size(); iCalo++) {
      //       std::cout << " caloCluster_position_Y_front.at(" << iCalo << "), pos_fibers_Y1.at(" << iCluster << ")  = " << caloCluster_position_Y_front.at(iCalo) << "," <<  pos_fibers_Y1.at(iCluster) << std::endl;
      hHS_HS1_Cal_front_Y->Fill(caloCluster_position_Y_front.at(iCalo), pos_fibers_Y1.at(iCluster));
      Y_hHS_HS1_Cal_front->Fill(caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster), i);
      Y_h1_HS1_Cal_front->Fill(caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_Y_back.size(); iCalo++) {
      hHS_HS1_Cal_back_Y->Fill(caloCluster_position_Y_back.at(iCalo), pos_fibers_Y1.at(iCluster));
      Y_hHS_HS1_Cal_back->Fill(caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster), i);
      Y_h1_HS1_Cal_back->Fill(caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster));
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_Y2.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_Y_front.size(); iCalo++) {
      hHS_HS2_Cal_front_Y->Fill(caloCluster_position_Y_front.at(iCalo), pos_fibers_Y2.at(iCluster));
      Y_hHS_HS2_Cal_front->Fill(caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster), i);
      Y_h1_HS2_Cal_front->Fill(caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_Y_back.size(); iCalo++) {
      hHS_HS2_Cal_back_Y->Fill(caloCluster_position_Y_back.at(iCalo), pos_fibers_Y2.at(iCluster));
      Y_hHS_HS2_Cal_back->Fill(caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster), i);
      Y_h1_HS2_Cal_back->Fill(caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster));
     }
    }
    
   }
   
   //---- Fill Calorimeter mapping
   for (int iCaloX = 0; iCaloX < caloCluster_position_X_front.size(); iCaloX++) {
    for (int iCaloY = 0; iCaloY < caloCluster_position_Y_front.size(); iCaloY++) {
     hHS_Cal_front->Fill(caloCluster_position_X_front.at(iCaloX),caloCluster_position_Y_front.at(iCaloY));
    }
   }
   for (int iCaloX = 0; iCaloX < caloCluster_position_X_back.size(); iCaloX++) {
    for (int iCaloY = 0; iCaloY < caloCluster_position_Y_back.size(); iCaloY++) {
     hHS_Cal_back->Fill(caloCluster_position_X_back.at(iCaloX),caloCluster_position_Y_back.at(iCaloY));
    }
   }
   
   //---- Fill Calorimeter mapping with module information only
   for (int iCaloX = 0; iCaloX < caloCluster_position_X_front_module.size(); iCaloX++) {
    for (int iCaloY = 0; iCaloY < caloCluster_position_Y_front_module.size(); iCaloY++) {
     hHS_Cal_front_module->Fill(caloCluster_position_X_front_module.at(iCaloX),caloCluster_position_Y_front_module.at(iCaloY));
//      std::cout << " calo cluster position (module) : " << caloCluster_position_X_front_module.at(iCaloX) << " , " << caloCluster_position_Y_front_module.at(iCaloY) << std::endl;
    }
   }
   for (int iCaloX = 0; iCaloX < caloCluster_position_X_back_module.size(); iCaloX++) {
    for (int iCaloY = 0; iCaloY < caloCluster_position_Y_back_module.size(); iCaloY++) {
     hHS_Cal_back_module->Fill(caloCluster_position_X_back_module.at(iCaloX),caloCluster_position_Y_back_module.at(iCaloY));
    }
   }
    
  }
  
  TPad* tempPad;
  
  //---- plot ----
  cc_hodo->Divide(2,2);
  TF1* fxy = new TF1 ("fxy","x",-20,20);
  
  cc_hodo->cd(1)->SetGrid();
  hHS_HS2_HS1_X->Draw("colz");
  hHS_HS2_HS1_X->GetXaxis()->SetTitle("X1");
  hHS_HS2_HS1_X->GetYaxis()->SetTitle("X2");
  fxy->Draw("same");
  
  cc_hodo->cd(2)->SetGrid();
  hHS_HS2_HS1_Y->Draw("colz");
  hHS_HS2_HS1_Y->GetXaxis()->SetTitle("Y1");
  hHS_HS2_HS1_Y->GetYaxis()->SetTitle("Y2");
  fxy->Draw("same");
  
  
  cc_hodo->cd(3)->SetGrid();
  hHS_HS1->Draw("colz");
  hHS_HS1->GetXaxis()->SetTitle("X");
  hHS_HS1->GetYaxis()->SetTitle("Y");
  tempPad = (TPad*) gPad;
  DrawShashlikModule(tempPad);
  
  cc_hodo->cd(4)->SetGrid();
  hHS_HS2->Draw("colz");
  hHS_HS2->GetXaxis()->SetTitle("X");
  hHS_HS2->GetYaxis()->SetTitle("Y");
  tempPad = (TPad*) gPad;
  DrawShashlikModule(tempPad);
  
  
  
  
  
  cc_Cal->Divide(2,2);
  cc_Cal->cd(1)->SetGrid();
  hHS_Cal_front->Draw("colz");
  hHS_Cal_front->GetXaxis()->SetTitle("cal X");
  hHS_Cal_front->GetYaxis()->SetTitle("cal Y");
  tempPad = (TPad*) gPad;
  DrawShashlikModule(tempPad);
  
  cc_Cal->cd(2)->SetGrid();
  hHS_Cal_back->Draw("colz");
  hHS_Cal_back->GetXaxis()->SetTitle("cal X");
  hHS_Cal_back->GetYaxis()->SetTitle("cal Y");
  tempPad = (TPad*) gPad;
  DrawShashlikModule(tempPad);
  
  cc_Cal->cd(3);
  hHS_Cal_front_module->Draw("colz");
  hHS_Cal_front_module->GetXaxis()->SetTitle("cal X");
  hHS_Cal_front_module->GetYaxis()->SetTitle("cal Y");
  tempPad = (TPad*) gPad;
  DrawShashlikModule(tempPad);
  
  cc_Cal->cd(4);
  hHS_Cal_back_module->Draw("colz");
  hHS_Cal_back_module->GetXaxis()->SetTitle("cal X");
  hHS_Cal_back_module->GetYaxis()->SetTitle("cal Y");
  tempPad = (TPad*) gPad;
  DrawShashlikModule(tempPad);
  

  
  
  TF1* fgaus = new TF1("fgaus","gaus(0)+pol0(3)",0,4);
  fgaus->SetParameter(1,1);
  fgaus->SetParameter(2,0.5);
  
  
  cc_DX->Divide(4,2);
  cc_DX->cd(1)->SetGrid();
  X_hHS_HS1_Cal_front->Draw("colz");
  X_hHS_HS1_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  X_hHS_HS1_Cal_front->GetYaxis()->SetTitle("event number");

  cc_DX->cd(2)->SetGrid();
  X_hHS_HS2_Cal_front->Draw("colz");
  X_hHS_HS2_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  X_hHS_HS2_Cal_front->GetYaxis()->SetTitle("event number");

  cc_DX->cd(5)->SetGrid();
  X_hHS_HS1_Cal_back->Draw("colz");
  X_hHS_HS1_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  X_hHS_HS1_Cal_back->GetYaxis()->SetTitle("event number");

  cc_DX->cd(6)->SetGrid();
  X_hHS_HS2_Cal_back->Draw("colz");
  X_hHS_HS2_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  X_hHS_HS2_Cal_back->GetYaxis()->SetTitle("event number");
  
  
  cc_DX->cd(3)->SetGrid();
  X_h1_HS1_Cal_front->Draw();
  X_h1_HS1_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  
  cc_DX->cd(4)->SetGrid();
  X_h1_HS2_Cal_front->Draw();
  X_h1_HS2_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  
  cc_DX->cd(7)->SetGrid();
  X_h1_HS1_Cal_back->Draw();
  X_h1_HS1_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  
  cc_DX->cd(8)->SetGrid();
  X_h1_HS2_Cal_back->Draw();
  X_h1_HS2_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  X_h1_HS2_Cal_back->Fit("fgaus","R");
  
  
  

  cc_X->Divide(2,2);
  
  cc_X->cd(1)->SetGrid();
  hHS_HS1_Cal_front_X->Draw("colz");
  hHS_HS1_Cal_front_X->GetXaxis()->SetTitle("Calo front");
  hHS_HS1_Cal_front_X->GetYaxis()->SetTitle("X1");
  fxy->Draw("same");
  
  cc_X->cd(2)->SetGrid();
  hHS_HS2_Cal_front_X->Draw("colz");
  hHS_HS2_Cal_front_X->GetXaxis()->SetTitle("Calo front");
  hHS_HS2_Cal_front_X->GetYaxis()->SetTitle("X2");
  fxy->Draw("same");
  
  cc_X->cd(3)->SetGrid();
  hHS_HS1_Cal_back_X->Draw("colz");
  hHS_HS1_Cal_back_X->GetXaxis()->SetTitle("Calo back");
  hHS_HS1_Cal_back_X->GetYaxis()->SetTitle("X1");
  fxy->Draw("same");
  
  cc_X->cd(4)->SetGrid();
  hHS_HS2_Cal_back_X->Draw("colz");
  hHS_HS2_Cal_back_X->GetXaxis()->SetTitle("Calo back");
  hHS_HS2_Cal_back_X->GetYaxis()->SetTitle("X2");
  fxy->Draw("same");
  
  
 
  
  cc_DY->Divide(4,2);
  cc_DY->cd(1)->SetGrid();
  Y_hHS_HS1_Cal_front->Draw("colz");
  Y_hHS_HS1_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  Y_hHS_HS1_Cal_front->GetYaxis()->SetTitle("event number");
  
  cc_DY->cd(2)->SetGrid();
  Y_hHS_HS2_Cal_front->Draw("colz");
  Y_hHS_HS2_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  Y_hHS_HS2_Cal_front->GetYaxis()->SetTitle("event number");
  
  cc_DY->cd(5)->SetGrid();
  Y_hHS_HS1_Cal_back->Draw("colz");
  Y_hHS_HS1_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  Y_hHS_HS1_Cal_back->GetYaxis()->SetTitle("event number");
  
  cc_DY->cd(6)->SetGrid();
  Y_hHS_HS2_Cal_back->Draw("colz");
  Y_hHS_HS2_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  Y_hHS_HS2_Cal_back->GetYaxis()->SetTitle("event number");
  
  
  cc_DY->cd(3)->SetGrid();
  Y_h1_HS1_Cal_front->Draw();
  Y_h1_HS1_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  
  cc_DY->cd(4)->SetGrid();
  Y_h1_HS2_Cal_front->Draw();
  Y_h1_HS2_Cal_front->GetXaxis()->SetTitle("calo - hodoscope");
  
  cc_DY->cd(7)->SetGrid();
  Y_h1_HS1_Cal_back->Draw();
  Y_h1_HS1_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  
  cc_DY->cd(8)->SetGrid();
  Y_h1_HS2_Cal_back->Draw();
  Y_h1_HS2_Cal_back->GetXaxis()->SetTitle("calo - hodoscope");
  
  
  
  cc_Y->Divide(2,2);
  
  cc_Y->cd(1)->SetGrid();
  hHS_HS1_Cal_front_Y->Draw("colz");
  hHS_HS1_Cal_front_Y->GetXaxis()->SetTitle("Calo front");
  hHS_HS1_Cal_front_Y->GetYaxis()->SetTitle("Y1");
  fxy->Draw("same");
  
  cc_Y->cd(2)->SetGrid();
  hHS_HS2_Cal_front_Y->Draw("colz");
  hHS_HS2_Cal_front_Y->GetXaxis()->SetTitle("Calo front");
  hHS_HS2_Cal_front_Y->GetYaxis()->SetTitle("Y2");
  fxy->Draw("same");
  
  cc_Y->cd(3)->SetGrid();
  hHS_HS1_Cal_back_Y->Draw("colz");
  hHS_HS1_Cal_back_Y->GetXaxis()->SetTitle("Calo back");
  hHS_HS1_Cal_back_Y->GetYaxis()->SetTitle("Y1");
  fxy->Draw("same");
  
  cc_Y->cd(4)->SetGrid();
  hHS_HS2_Cal_back_Y->Draw("colz");
  hHS_HS2_Cal_back_Y->GetXaxis()->SetTitle("Calo back");
  hHS_HS2_Cal_back_Y->GetYaxis()->SetTitle("Y2");
  fxy->Draw("same");
  
  gMyRootApp->Run(); 
  
}




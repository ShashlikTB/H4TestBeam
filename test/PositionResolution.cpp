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

#include "include/CaloCluster.h"


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






  

int main(int argc, char**argv){
 
 std::string input_file;
 int maxEvents = -1;
 int doFiber = 0;
 float table_x_reference = 200; //---- mm
 float table_y_reference = 350; //---- mm
 float table_x = 200; //---- mm
 float table_y = 350; //---- mm
 
 float w0 = 5.0;
 //---- configuration
 
 int c;
 while ((c = getopt (argc, argv, "i:m:f:x:y:w:")) != -1)
  switch (c) {
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
   case 'w':
    w0 =  atof(optarg);
    break;
    
   case '?':
    if (optopt == 'i' || optopt == 'm' || optopt == 'f' || optopt == 'x' || optopt == 'y' || optopt == 'w')
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
   std::cout << "  >> file: " << input_files_vector.at(i) << std::endl;
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
  
//   TBranch *branch_event = H4tree->GetBranch("tbevent");
//   branch_event->SetAddress(&tbevent);
//   TBranch *branch_spill = H4tree->GetBranch("tbspill");
//   branch_spill->SetAddress(&tbspill);
  
  H4tree->SetBranchAddress("tbevent", &tbevent);
  H4tree->SetBranchAddress("tbspill", &tbspill);

  
  
  TApplication* gMyRootApp = new TApplication("My ROOT Application", &argc, argv);
  
  TCanvas* cc_X = new TCanvas ("cc_X","",800,600);
  TCanvas* cc_Y = new TCanvas ("cc_Y","",800,600);
  TCanvas* cc_R = new TCanvas ("cc_R","",800,600);
  
  TH1F *hHS_HS1_Cal_front_X  = new TH1F("hHS_HS1_Cal_front_X", "Hodoscope 1 vs Cal front X", 128, -32, 32);
  TH1F *hHS_HS2_Cal_front_X  = new TH1F("hHS_HS2_Cal_front_X", "Hodoscope 2 vs Cal front X", 128, -32, 32);  
  TH1F *hHS_HS1_Cal_back_X  = new TH1F("hHS_HS1_Cal_back_X", "Hodoscope 1 vs Cal back X", 128, -32, 32);
  TH1F *hHS_HS2_Cal_back_X  = new TH1F("hHS_HS2_Cal_back_X", "Hodoscope 2 vs Cal back X", 128, -32, 32);  
  
  TH1F *hHS_HS1_Cal_front_Y  = new TH1F("hHS_HS1_Cal_front_Y", "Hodoscope 1 vs Cal front Y", 128, -32, 32);
  TH1F *hHS_HS2_Cal_front_Y  = new TH1F("hHS_HS2_Cal_front_Y", "Hodoscope 2 vs Cal front Y", 128, -32, 32);  
  TH1F *hHS_HS1_Cal_back_Y  = new TH1F("hHS_HS1_Cal_back_Y", "Hodoscope 1 vs Cal back Y", 128, -32, 32);
  TH1F *hHS_HS2_Cal_back_Y  = new TH1F("hHS_HS2_Cal_back_Y", "Hodoscope 2 vs Cal back Y", 128, -32, 32);  
  
  TH1F *hHS_HS1_Cal_front_R  = new TH1F("hHS_HS1_Cal_front_R", "Hodoscope 1 vs Cal front R", 128, 0, 32);
  TH1F *hHS_HS2_Cal_front_R  = new TH1F("hHS_HS2_Cal_front_R", "Hodoscope 2 vs Cal front R", 128, 0, 32);  
  TH1F *hHS_HS1_Cal_back_R  = new TH1F("hHS_HS1_Cal_back_R", "Hodoscope 1 vs Cal back R", 128, 0, 32);
  TH1F *hHS_HS2_Cal_back_R  = new TH1F("hHS_HS2_Cal_back_R", "Hodoscope 2 vs Cal back R", 128, 0, 32);  
  
  double Energy_Beam;
  
  
  bool haverechits = false;
  std::vector<TBRecHit> *rechits=0;
  if(H4tree->GetListOfBranches()->FindObject("tbrechits")) {
   std::cout << " found rechits " << std::endl;
   H4tree->SetBranchAddress("tbrechits",&rechits);
   haverechits = true;
  }
  
  
  CaloCluster* caloCluster = new CaloCluster();
  caloCluster->setW0(w0);
  caloCluster->setInterCalibrationConstants("data/InterCalibrationConstants.txt");
  

  ///---- loop ----
  for (int i=0; i<nEntries; i++) {
   
   if ((i%1000)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree->GetEntry(i);
   

   float table_x_shift = tbspill->GetTableX();
   float table_y_shift = tbspill->GetTableY();
   
   if ( ((table_x_reference - table_x_shift) != table_x) || (i == 0) ) {
    std::cout << " Table: " << std::endl;
    std::cout << "   x = " << table_x_reference - table_x_shift << " mm " << std::endl;
    std::cout << "   y = " << table_y_reference - table_y_shift << " mm " << std::endl;
   }
   
   table_x = table_x_reference - table_x_shift;
   table_y = table_y_reference - table_y_shift;
   
   
   Energy_Beam = tbspill->GetMomentum();

   
   //---- calorimeter data
   if (i==0) caloCluster->setMapperEpoch(tbevent->GetTimeStamp());
   
   
   std::vector<float> caloCluster_position_X_front;
   std::vector<float> caloCluster_position_Y_front;
   std::vector<float> caloCluster_Energy_front;
   std::vector<float> caloCluster_Energies_front;
   
   caloCluster->doCalorimeterReconstruction( rechits, 1, 30, doFiber);
   
   caloCluster_position_X_front.push_back( caloCluster->getPositionX() );
   caloCluster_position_Y_front.push_back( caloCluster->getPositionY() );
   caloCluster_Energy_front.push_back( caloCluster->getEnergy() );
   caloCluster_Energies_front = caloCluster->getCaloClusterComponents();
   
   std::vector<float> caloCluster_position_X_back;
   std::vector<float> caloCluster_position_Y_back;
   std::vector<float> caloCluster_Energy_back;
   std::vector<float> caloCluster_Energies_back;;
   
     
   caloCluster->doCalorimeterReconstruction( rechits, -1, 30, doFiber);
   
   caloCluster_position_X_back.push_back( caloCluster->getPositionX() );
   caloCluster_position_Y_back.push_back( caloCluster->getPositionY() );
   caloCluster_Energy_back.push_back( caloCluster->getEnergy() );
   caloCluster_Energies_back = caloCluster->getCaloClusterComponents();
   
//    std::cout << " caloCluster_position_X_back.size () = " << caloCluster_position_X_back.size() << " ::: " ;
//    std::cout << " caloCluster_position_Y_back.size () = " << caloCluster_position_Y_back.size() << std::endl;
   
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
   
   
   //---- now merge and compare
   
   float mindistance = 1000;
   float mindistance_X = 1000;
   float mindistance_Y = 1000;
   
   //---- among all the hodoscope clusters, keep the closest one -> multiple electrons?
   
   //---- X
   for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_X1.size(); iCluster++) {
     if (mindistance > (caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster))) {
      mindistance = caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS1_Cal_front_X->Fill(mindistance);
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_X2.size(); iCluster++) {
     if (mindistance > (caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster))) {
      mindistance = caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS2_Cal_front_X->Fill(mindistance);
    
   }
   
   for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_X1.size(); iCluster++) {
     if (mindistance > (caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster))) {
      mindistance = caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS1_Cal_back_X->Fill(mindistance);
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_X2.size(); iCluster++) {
     if (mindistance > (caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster))) {
      mindistance = caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS2_Cal_back_X->Fill(mindistance);
    
   }
      
   //---- Y
   for (int iCalo = 0; iCalo < caloCluster_position_Y_front.size(); iCalo++) {
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_Y1.size(); iCluster++) {
     if (mindistance > (caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster))) {
      mindistance = caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS1_Cal_front_Y->Fill(mindistance);
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_Y2.size(); iCluster++) {
     if (mindistance > (caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster))) {
      mindistance = caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS2_Cal_front_Y->Fill(mindistance);
    
   }
   
   for (int iCalo = 0; iCalo < caloCluster_position_Y_back.size(); iCalo++) {
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_Y1.size(); iCluster++) {
     if (mindistance > (caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster))) {
      mindistance = caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS1_Cal_back_Y->Fill(mindistance);
    
    mindistance = 1000;
    for (int iCluster = 0; iCluster < pos_fibers_Y2.size(); iCluster++) {
     if (mindistance > (caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster))) {
      mindistance = caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster);
     }
    }
    if (mindistance != 1000) hHS_HS2_Cal_back_Y->Fill(mindistance);
    
   }
   
   
   //---- R
   for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
    mindistance_X = 1000;   
    mindistance_Y = 1000;   
    
    for (int iCluster = 0; iCluster < pos_fibers_X1.size(); iCluster++) {
     if (mindistance_X > (caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster))) {
      mindistance_X = caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster);
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_Y1.size(); iCluster++) {
     if (mindistance_Y > (caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster))) {
      mindistance_Y = caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster);
     }
    }
    
    if ((mindistance_X != 1000) && (mindistance_Y != 1000)) {
     hHS_HS1_Cal_front_R->Fill(sqrt(mindistance_X*mindistance_X + mindistance_Y*mindistance_Y));
    }
   }
   
   for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
    mindistance_X = 1000;   
    mindistance_Y = 1000;   
    
    for (int iCluster = 0; iCluster < pos_fibers_X2.size(); iCluster++) {
     if (mindistance_X > (caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster))) {
      mindistance_X = caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster);
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_Y2.size(); iCluster++) {
     if (mindistance_Y > (caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster))) {
      mindistance_Y = caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster);
     }
    }
    
    if ((mindistance_X != 1000) && (mindistance_Y != 1000)) {
     hHS_HS2_Cal_front_R->Fill(sqrt(mindistance_X*mindistance_X + mindistance_Y*mindistance_Y));
    }
   }
   
   
   for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
    mindistance_X = 1000;   
    mindistance_Y = 1000;   
    
    for (int iCluster = 0; iCluster < pos_fibers_X1.size(); iCluster++) {
     if (mindistance_X > (caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster))) {
      mindistance_X = caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster);
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_Y1.size(); iCluster++) {
     if (mindistance_Y > (caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster))) {
      mindistance_Y = caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster);
     }
    }
    
    if ((mindistance_X != 1000) && (mindistance_Y != 1000)) {
     hHS_HS1_Cal_back_R->Fill(sqrt(mindistance_X*mindistance_X + mindistance_Y*mindistance_Y));
    }
   }
   
   for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
    mindistance_X = 1000;   
    mindistance_Y = 1000;   
    
    for (int iCluster = 0; iCluster < pos_fibers_X2.size(); iCluster++) {
     if (mindistance_X > (caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster))) {
      mindistance_X = caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster);
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_Y2.size(); iCluster++) {
     if (mindistance_Y > (caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster))) {
      mindistance_Y = caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster);
     }
    }
    
    if ((mindistance_X != 1000) && (mindistance_Y != 1000)) {
     hHS_HS2_Cal_back_R->Fill(sqrt(mindistance_X*mindistance_X + mindistance_Y*mindistance_Y));
    }
   }
   
  } //---- end loop on events
 
  
  
//   TPad* tempPad;
  
  //---- plot ----
  
  TF1* fgaus = new TF1("fgaus","gaus(0)+pol2(3)",-10,10);
  fgaus->SetParameter(0,10);
  fgaus->SetParameter(1,0.0);
  fgaus->SetParLimits(1,-10,10);
  fgaus->SetParameter(2,0.5);
  fgaus->SetParLimits(2,0.1,3.0);
  
  
  fgaus->SetParameter(4,0.0);
  fgaus->SetParameter(5,0.0);
  
  float s_X_1_front;
  float s_Y_1_front;
  float s_R_1_front;
  float s_X_2_front;
  float s_Y_2_front;
  float s_R_2_front;

  float s_X_1_back;
  float s_Y_1_back;
  float s_R_1_back;
  float s_X_2_back;
  float s_Y_2_back;
  float s_R_2_back;
  
  cc_X->Divide(2,2);
  
  cc_X->cd(1)->SetGrid();
  hHS_HS1_Cal_front_X->Draw();
  hHS_HS1_Cal_front_X->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS1_Cal_front_X->Fit("fgaus","R");
  s_X_1_front = fgaus->GetParameter(2);
  
  cc_X->cd(2)->SetGrid();
  hHS_HS2_Cal_front_X->Draw();
  hHS_HS2_Cal_front_X->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS2_Cal_front_X->Fit("fgaus","R");
  s_X_2_front = fgaus->GetParameter(2);
  
  cc_X->cd(3)->SetGrid();
  hHS_HS1_Cal_back_X->Draw();
  hHS_HS1_Cal_back_X->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS1_Cal_back_X->Fit("fgaus","R");
  s_X_1_back = fgaus->GetParameter(2);
  
  cc_X->cd(4)->SetGrid();
  hHS_HS2_Cal_back_X->Draw();
  hHS_HS2_Cal_back_X->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS2_Cal_back_X->Fit("fgaus","R");
  s_X_2_back = fgaus->GetParameter(2);
  
  
  cc_Y->Divide(2,2);
  
  cc_Y->cd(1)->SetGrid();
  hHS_HS1_Cal_front_Y->Draw();
  hHS_HS1_Cal_front_Y->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS1_Cal_front_Y->Fit("fgaus","R");
  s_Y_1_front = fgaus->GetParameter(2);
  
  cc_Y->cd(2)->SetGrid();
  hHS_HS2_Cal_front_Y->Draw();
  hHS_HS2_Cal_front_Y->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS2_Cal_front_Y->Fit("fgaus","R");
  s_Y_2_front = fgaus->GetParameter(2);
  
  cc_Y->cd(3)->SetGrid();
  hHS_HS1_Cal_back_Y->Draw();
  hHS_HS1_Cal_back_Y->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS1_Cal_back_Y->Fit("fgaus","R");
  s_Y_1_back = fgaus->GetParameter(2);
  
  cc_Y->cd(4)->SetGrid();
  hHS_HS2_Cal_back_Y->Draw();
  hHS_HS2_Cal_back_Y->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS2_Cal_back_Y->Fit("fgaus","R");
  s_Y_2_back = fgaus->GetParameter(2);
  
  
  cc_R->Divide(2,2);
  
  fgaus->SetParameter(0,10);
  fgaus->SetParameter(1,0.0);
  fgaus->SetParLimits(1,-10,10);
  fgaus->SetParameter(2,0.5);
  fgaus->SetParLimits(2,0.1,3.0);
  
  fgaus->SetParameter(4,0.0);
  fgaus->SetParameter(5,0.0);
  
  cc_R->cd(1)->SetGrid();
  hHS_HS1_Cal_front_R->Draw();
  hHS_HS1_Cal_front_R->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS1_Cal_front_R->Fit("fgaus","R");
  hHS_HS1_Cal_front_R->Fit("fgaus","R");
  hHS_HS1_Cal_front_R->Fit("fgaus","R");
  s_R_1_front = fgaus->GetParameter(2);
  
  cc_R->cd(2)->SetGrid();
  hHS_HS2_Cal_front_R->Draw();
  hHS_HS2_Cal_front_R->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS2_Cal_front_R->Fit("fgaus","R");
  hHS_HS2_Cal_front_R->Fit("fgaus","R");
  hHS_HS2_Cal_front_R->Fit("fgaus","R");
  s_R_2_front = fgaus->GetParameter(2);
  
  cc_R->cd(3)->SetGrid();
  hHS_HS1_Cal_back_R->Draw();
  hHS_HS1_Cal_back_R->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS1_Cal_back_R->Fit("fgaus","R");
  hHS_HS1_Cal_back_R->Fit("fgaus","R");
  hHS_HS1_Cal_back_R->Fit("fgaus","R");
  s_R_1_back = fgaus->GetParameter(2);
  
  cc_R->cd(4)->SetGrid();
  hHS_HS2_Cal_back_R->Draw();
  hHS_HS2_Cal_back_R->GetXaxis()->SetTitle("calo - hodoscope");
  hHS_HS2_Cal_back_R->Fit("fgaus","R");
  hHS_HS2_Cal_back_R->Fit("fgaus","R");
  hHS_HS2_Cal_back_R->Fit("fgaus","R");
  s_R_2_back = fgaus->GetParameter(2);
  
  
  ofstream myfile;
  myfile.open ("position_resolution.txt",  std::ofstream::app);
  myfile << Energy_Beam << "   ";
  myfile << "  " << s_X_1_front;
  myfile << "  " << s_Y_1_front;
  myfile << "  " << s_R_1_front;
  myfile << "  " << s_X_2_front;
  myfile << "  " << s_Y_2_front;
  myfile << "  " << s_R_2_front;
  myfile << "  " ;
  myfile << "  " << s_X_1_back;
  myfile << "  " << s_Y_1_back;
  myfile << "  " << s_R_1_back;
  myfile << "  " << s_X_2_back;
  myfile << "  " << s_Y_2_back;
  myfile << "  " << s_R_2_back;
  myfile << std::endl;
  myfile.close(); 
  
  
  
  
  gMyRootApp->Run(); 
  
}




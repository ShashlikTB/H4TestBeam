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

#include "include/HodoscopeMap.h"


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
 std::string output_file;
 int maxEvents = -1;
 int doFiber = 0;
 float table_x = 200; //---- mm
 float table_y = 350; //---- mm
 
 float w0 = 5.0;
 //---- configuration
 
 int c;
 while ((c = getopt (argc, argv, "i:o:m:f:w:")) != -1)
  switch (c)
  {
   case 'i': //---- input
    input_file = string(optarg);
    break;
   case 'o': //---- output
    output_file = string(optarg);
    break;
   case 'm':
    maxEvents =  atoi(optarg);
    break;
   case 'f':
    doFiber =  atoi(optarg);
    break;
   case 'w':
    w0 =  atof(optarg);
    break;
    
   case '?':
    if (optopt == 'i' || optopt == 'o' || optopt == 'm' || optopt == 'f' || optopt == 'w')
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
   std::cout << " input: " << input_files_vector.at(i) << std::endl;
  }
  
  //---- configuration (end)
  
  
  //---- input file
  
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
  
  
  TH1F *X_h1_HS1_Cal_front  = new TH1F("X_h1_HS1_Cal_front", "X Hodoscope 1 vs Cal front ", 256, -32, 32);
  TH1F *X_h1_HS2_Cal_front  = new TH1F("X_h1_HS2_Cal_front", "X Hodoscope 2 vs Cal front ", 256, -32, 32);
  TH1F *X_h1_HS1_Cal_back   = new TH1F("X_h1_HS1_Cal_back",  "X Hodoscope 1 vs Cal back " , 256, -32, 32);
  TH1F *X_h1_HS2_Cal_back   = new TH1F("X_h1_HS2_Cal_back",  "X Hodoscope 2 vs Cal back " , 256, -32, 32);
    
  TH1F *Y_h1_HS1_Cal_front  = new TH1F("Y_h1_HS1_Cal_front", "Y Hodoscope 1 vs Cal front ", 256, -32, 32);
  TH1F *Y_h1_HS2_Cal_front  = new TH1F("Y_h1_HS2_Cal_front", "Y Hodoscope 2 vs Cal front ", 256, -32, 32);
  TH1F *Y_h1_HS1_Cal_back   = new TH1F("Y_h1_HS1_Cal_back",  "Y Hodoscope 1 vs Cal back " , 256, -32, 32);
  TH1F *Y_h1_HS2_Cal_back   = new TH1F("Y_h1_HS2_Cal_back",  "Y Hodoscope 2 vs Cal back " , 256, -32, 32);
  
  TH1F *Energy_Cal_back   = new TH1F("Energy_Cal_back",  "Energy Cal back " , 3000, 0, 300000);
  TH1F *Energy_Cal_front  = new TH1F("Energy_Cal_front", "Energy Cal front" , 3000, 0, 300000);
  
  
  bool haverechits = false;
//   std::vector<TBRecHit> *rechits = 0;
  std::vector<TBRecHit> *rechits = new vector<TBRecHit>;
  if(H4tree->GetListOfBranches()->FindObject("tbrechits")) {
   std::cout << " found rechits " << std::endl;
   H4tree->SetBranchAddress("tbrechits",&rechits);
   haverechits = true;
  }
  
  
  
  
  
  
  //---- output file
  
  TFile* output_file_root = new TFile (output_file.c_str(),"RECREATE");
  TTree* outtree = new TTree("t1041","t1041");
  
  outtree->Branch("tbspill", "TBSpill", &tbspill, 64000, 0);
  outtree->Branch("tbevent", "TBEvent", &tbevent, 64000, 0);
  if (haverechits) {
   outtree->Branch("tbrechits","std::vector<TBRecHit>",&rechits);
  }
  
  

  
  
//   Mapper *mapper = Mapper::Instance();
  
  CaloCluster* caloCluster = new CaloCluster();
  caloCluster->setW0(w0);
  
  for (int i=0; i<nEntries; i++) {
   
   if ((i%1000)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree->GetEntry(i);
   
   //---- calorimeter data
   if (i==0) caloCluster->setMapperEpoch(tbevent->GetTimeStamp());

   std::vector<float> caloCluster_position_X_front;
   std::vector<float> caloCluster_position_Y_front;
   std::vector<float> caloCluster_Energy_front;
   
//    void doCalorimeterReconstruction( std::vector<TBRecHit>* rechits, int face, float maxDR, int fiberLevel = 0);
   caloCluster->doCalorimeterReconstruction( rechits, 1, 30, doFiber);
   
   caloCluster_position_X_front.push_back( caloCluster->getPositionX() );
   caloCluster_position_Y_front.push_back( caloCluster->getPositionY() );
   caloCluster_Energy_front.push_back( caloCluster->getEnergy() );
   
   
   std::vector<float> caloCluster_position_X_back;
   std::vector<float> caloCluster_position_Y_back;
   std::vector<float> caloCluster_Energy_back;
   
   
   caloCluster->doCalorimeterReconstruction( rechits, -1, 30, doFiber);
   
   caloCluster_position_X_back.push_back( caloCluster->getPositionX() );
   caloCluster_position_Y_back.push_back( caloCluster->getPositionY() );
   caloCluster_Energy_back.push_back( caloCluster->getEnergy() );
   
   
   
   //---- modular level DR = 5 mm
   std::vector<float> caloCluster_position_X_front_module;
   std::vector<float> caloCluster_position_Y_front_module;
   std::vector<float> caloCluster_Energy_front_module;
   
   caloCluster->doCalorimeterReconstruction( rechits, 1, 5, doFiber);

   caloCluster_position_X_front_module.push_back( caloCluster->getPositionX() );
   caloCluster_position_Y_front_module.push_back( caloCluster->getPositionY() );
   caloCluster_Energy_front_module.push_back( caloCluster->getEnergy() );
      
   
   std::vector<float> caloCluster_position_X_back_module;
   std::vector<float> caloCluster_position_Y_back_module;
   std::vector<float> caloCluster_Energy_back_module;
   
   caloCluster->doCalorimeterReconstruction( rechits, -1, 5, doFiber);
   
   caloCluster_position_X_back_module.push_back( caloCluster->getPositionX() );
   caloCluster_position_Y_back_module.push_back( caloCluster->getPositionY() );
   caloCluster_Energy_back_module.push_back( caloCluster->getEnergy() );


   
   for (unsigned int iEnergy = 0; iEnergy<caloCluster_Energy_back.size(); iEnergy++) {
    Energy_Cal_back->Fill(caloCluster_Energy_back.at(iEnergy));
//     std::cout << " caloCluster_Energy_back.at(" << iEnergy << ") = " << caloCluster_Energy_back.at(iEnergy) << std::endl;
   }
   for (unsigned int iEnergy = 0; iEnergy<caloCluster_Energy_front.size(); iEnergy++) {
    Energy_Cal_front->Fill(caloCluster_Energy_front.at(iEnergy));
   }
   
   
   
   
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
   if (pos_fibers_X1.size() > 1) {
    
    //---- X
    for (int iCluster = 0; iCluster < pos_fibers_X1.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
      X_h1_HS1_Cal_front->Fill(caloCluster_position_X_front.at(iCalo) - pos_fibers_X1.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
      X_h1_HS1_Cal_back->Fill(caloCluster_position_X_back.at(iCalo) - pos_fibers_X1.at(iCluster));
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_X2.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_X_front.size(); iCalo++) {
      X_h1_HS2_Cal_front->Fill(caloCluster_position_X_front.at(iCalo) - pos_fibers_X2.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_X_back.size(); iCalo++) {
      X_h1_HS2_Cal_back->Fill(caloCluster_position_X_back.at(iCalo) - pos_fibers_X2.at(iCluster));
     }
    }
    
    //---- Y
    for (int iCluster = 0; iCluster < pos_fibers_Y1.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_Y_front.size(); iCalo++) {
      Y_h1_HS1_Cal_front->Fill(caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y1.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_Y_back.size(); iCalo++) {
      Y_h1_HS1_Cal_back->Fill(caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y1.at(iCluster));
     }
    }
    for (int iCluster = 0; iCluster < pos_fibers_Y2.size(); iCluster++) {
     for (int iCalo = 0; iCalo < caloCluster_position_Y_front.size(); iCalo++) {
      Y_h1_HS2_Cal_front->Fill(caloCluster_position_Y_front.at(iCalo) - pos_fibers_Y2.at(iCluster));
     }
     for (int iCalo = 0; iCalo < caloCluster_position_Y_back.size(); iCalo++) {
      Y_h1_HS2_Cal_back->Fill(caloCluster_position_Y_back.at(iCalo) - pos_fibers_Y2.at(iCluster));
     }
    }
    
   }
   

  }
  

  //---- calculate the shifts in x and y
  float x1_shift = X_h1_HS1_Cal_front->GetBinCenter(X_h1_HS1_Cal_front->GetMaximumBin());  //----> trust more the front side
  float x2_shift = X_h1_HS2_Cal_front->GetBinCenter(X_h1_HS2_Cal_front->GetMaximumBin());
//   float x1_shift = X_h1_HS1_Cal_back->GetBinCenter(X_h1_HS1_Cal_back->GetMaximumBin());  //----> trust more the back side
//   float x2_shift = X_h1_HS2_Cal_back->GetBinCenter(X_h1_HS2_Cal_back->GetMaximumBin());
  
  float average_x_shift = (x1_shift + x2_shift) / 2.;
  average_x_shift = x2_shift; //---- use only hodoscope 2
  
  float y1_shift = Y_h1_HS1_Cal_front->GetBinCenter(Y_h1_HS1_Cal_front->GetMaximumBin());
  float y2_shift = Y_h1_HS2_Cal_front->GetBinCenter(Y_h1_HS2_Cal_front->GetMaximumBin());
//   float y1_shift = Y_h1_HS1_Cal_back->GetBinCenter(Y_h1_HS1_Cal_back->GetMaximumBin());
//   float y2_shift = Y_h1_HS2_Cal_back->GetBinCenter(Y_h1_HS2_Cal_back->GetMaximumBin());
  
  float average_y_shift = (y1_shift + y2_shift) / 2.;
  average_y_shift = y2_shift; //---- use only hodoscope 2
  
  
  HodoscopeMap hodoMap;
  std::pair<float, float> real_shift = hodoMap.GetCloser(average_x_shift, average_y_shift);

  std::cout << " -------------------- " << std::endl;
  std::cout << " Table position shift " << std::endl;
  std::cout << " average_x_shift = " << average_x_shift << std::endl;
  std::cout << " average_y_shift = " << average_y_shift << std::endl;
  std::cout << " -- " << std::endl;
  std::cout << " real_x_shift = " << real_shift.first << std::endl;
  std::cout << " real_y_shift = " << real_shift.second << std::endl;
  
  //--- now fill the new tree
  std::cout << " ============= " << std::endl;
  std::cout << " now filling the new tree ..." << std::endl;
  
  float energy_back  = Energy_Cal_back->GetBinCenter(Energy_Cal_back->GetMaximumBin());
  float energy_front = Energy_Cal_front->GetBinCenter(Energy_Cal_front->GetMaximumBin());
  std::cout << " Energy = " << energy_back << " , " << energy_front << std::endl;
  
  float real_energy = 0;
  //---- transform in GeV
  real_energy =  energy_front / 100.; //---- 50 GeV --> 5000 adc
  //---- round to beam energy
  float beamEnergies[6] = {10, 20, 50, 100, 150, 200};
  float min_dEnergy = 10000;
  int min_dEnergy_index = -1;
  for (int iEnergy=0; iEnergy<6; iEnergy++) {
   float temp_dEnergy = fabs(beamEnergies[iEnergy] - real_energy) ;
   if (temp_dEnergy < min_dEnergy) {
    min_dEnergy = temp_dEnergy;
    min_dEnergy_index = iEnergy;
   }
  }
  std::cout << " Energy [GeV] = " << real_energy << " --> " << beamEnergies[min_dEnergy_index] << std::endl;
  
  
  for (int i=0; i<nEntries; i++) {
   
   if ((i%1000)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree->GetEntry(i);
   
   ///---- update position
   tbspill -> SetTableX (real_shift.first);
   tbspill -> SetTableY (real_shift.second);   
//    tbspill -> SetTableX (table_x - average_x_shift);
//    tbspill -> SetTableY (table_y - average_y_shift);
   
   ///---- update momentum -> beam energy
   tbspill -> SetMomentum(beamEnergies[min_dEnergy_index]);
   
   outtree->Fill(); 
  }
  
  
  
  
  //---- save output
  
  output_file_root->cd();
  outtree->Write();
  output_file_root->Close();
  
}




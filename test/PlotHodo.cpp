#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TApplication.h"
#include "TChain.h"
#include "TTreeFormula.h"
#include "TCanvas.h"

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




#define hodoX1 0
#define hodoY1 1
#define hodoX2 2
#define hodoY2 3




//---- boost
// #include "boost/program_options.hpp"
// #include "boost/program_options/options_description.hpp"

// namespace po = boost::program_options;

#include <sstream> 

void TransformFibers(std::map<std::pair<int,int>, int > fibers, std::vector <int>& fibers_X1, std::vector <int>& fibers_X2, std::vector <int>& fibers_Y1, std::vector <int>& fibers_Y2){
  
 std::pair<int,int> fibers_mappairY1;
 fibers_mappairY1.first  = hodoY1;
 std::pair<int,int> fibers_mappairY2;
 fibers_mappairY2.first  = hodoY2;
 
 for(int iBinY=0;iBinY<64;iBinY++){
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


void doHodoReconstruction( std::vector<int> input_values, std::vector<int>& nFibres, std::vector<float>& cluster_position) {
 std::vector<HodoCluster*> clusters = getHodoClusters( input_values );
 for( unsigned i=0; i<clusters.size(); ++i ) {
  nFibres.push_back( clusters[i]->getSize() );
  cluster_position.push_back( clusters[i]->getPosition() );
 }
}







int main(int argc, char**argv){
 
 std::string input_file;
 int maxEvents = -1;
 
 //---- configuration
 
 int c;
 while ((c = getopt (argc, argv, "i:m:")) != -1)
  switch (c)
  {
   case 'i': //---- input
    input_file = string(optarg);
    break;
   case 'm':
    maxEvents =  atoi(optarg);
    break;
    
   case '?':
    if (optopt == 'i' || optopt == 'm')
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
  
  TCanvas* cc = new TCanvas ("cc","",800,600);
  
  TH2F *hHS_HS2_HS1_X  = new TH2F("hHS_HS2_HS1_X", "Hodoscope 2 vs Hodoscope 1 X", 64, -32, 32, 64, -32, 32);
  TH2F *hHS_HS2_HS1_Y  = new TH2F("hHS_HS2_HS1_Y", "Hodoscope 2 vs Hodoscope 1 Y", 64, -32, 32, 64, -32, 32);
  
  TH2F *num_hHS_HS2_HS1_X  = new TH2F("num_hHS_HS2_HS1_X", "Hodoscope 2 vs Hodoscope 1 X", 10, 0, 10 , 10, 0, 10 );
  TH2F *num_hHS_HS2_HS1_Y  = new TH2F("num_hHS_HS2_HS1_Y", "Hodoscope 2 vs Hodoscope 1 Y", 10, 0, 10 , 10, 0, 10 );
  
  
  for (int i=0; i<nEntries; i++) {
   
   if ((i%100)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree->GetEntry(i);
   
   
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
   
   std::vector <float> pos_fibers_X1;
   std::vector <float> pos_fibers_X2;
   std::vector <float> pos_fibers_Y1;
   std::vector <float> pos_fibers_Y2;
   
   
   doHodoReconstruction( fibers_X1, n_fibers_X1, pos_fibers_X1 );
   doHodoReconstruction( fibers_X2, n_fibers_X2, pos_fibers_X2 );
   doHodoReconstruction( fibers_Y1, n_fibers_Y1, pos_fibers_Y1 );
   doHodoReconstruction( fibers_Y2, n_fibers_Y2, pos_fibers_Y2 );
   
   for (int iCluster1 = 0; iCluster1 < pos_fibers_X1.size(); iCluster1++) {
    for (int iCluster2 = 0; iCluster2 < pos_fibers_X2.size(); iCluster2++) {
     hHS_HS2_HS1_X->Fill(pos_fibers_X1.at(iCluster1),pos_fibers_X2.at(iCluster2));
     std::cout << " pos_fibers_X:: " << pos_fibers_X1.at(iCluster1) << " :: " << pos_fibers_X2.at(iCluster2) << std::endl;
    }
   }
   
   for (int iCluster1 = 0; iCluster1 < pos_fibers_Y1.size(); iCluster1++) {
    for (int iCluster2 = 0; iCluster2 < pos_fibers_Y2.size(); iCluster2++) {
     hHS_HS2_HS1_Y->Fill(pos_fibers_Y1.at(iCluster1),pos_fibers_Y2.at(iCluster2));
    }
   }
   
   num_hHS_HS2_HS1_X->Fill(pos_fibers_X1.size(), pos_fibers_X2.size());
   num_hHS_HS2_HS1_Y->Fill(pos_fibers_Y1.size(), pos_fibers_Y2.size());
   
  }
  
  cc->Divide(2,2);
 
  TF1* fxy = new TF1 ("fxy","x",-20,20);
  cc->cd(1)->SetGrid();
  hHS_HS2_HS1_X->Draw("colz");
  hHS_HS2_HS1_X->GetXaxis()->SetTitle("X1");
  hHS_HS2_HS1_X->GetYaxis()->SetTitle("X2");
  fxy->Draw("same");

  cc->cd(2)->SetGrid();
  hHS_HS2_HS1_Y->Draw("colz");
  hHS_HS2_HS1_Y->GetXaxis()->SetTitle("Y1");
  hHS_HS2_HS1_Y->GetYaxis()->SetTitle("Y2");
  fxy->Draw("same");
  
  cc->cd(3)->SetGrid();
  num_hHS_HS2_HS1_X->Draw("colz");
  num_hHS_HS2_HS1_X->GetXaxis()->SetTitle("number of clusters 1");
  num_hHS_HS2_HS1_X->GetYaxis()->SetTitle("number of clusters 2");
  
//   fxy->Draw("same");
  cc->cd(4)->SetGrid();
  num_hHS_HS2_HS1_Y->Draw("colz");
  num_hHS_HS2_HS1_Y->GetXaxis()->SetTitle("number of clusters 1");
  num_hHS_HS2_HS1_Y->GetYaxis()->SetTitle("number of clusters 2");
  //   fxy->Draw("same");  
  
  gMyRootApp->Run(); 
  
}




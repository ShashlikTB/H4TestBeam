#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
// #include "TBranch.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
// #include "TBEvent.h"
// #include "Mapper.h"



const int MAXADC=4095;


#define hodoX1 0
#define hodoY1 1
#define hodoX2 2
#define hodoY2 3

// #include "rootscript/plotTools.h"

// inputs data file and event in file to display (default is to integrate all)
void beamPositionOnly(TString fdat, int firstTime = 1){
 
 gInterpreter->GenerateDictionary("std::map<std::pair<int,int>, int >","map");
 
 if (firstTime) {
//   std::cout << "doing this!" << std::endl;
  gROOT->ProcessLine("#include \"include/TBEvent.h\"");
  gROOT->ProcessLine("#include \"include/TBRecHit.h\"");
  gROOT->ProcessLine("#include \"include/Mapper.h\"");
  gROOT->ProcessLine("#include <map>");
 }
 
 gStyle->SetOptStat(0);
 
 TFile *f = new TFile(fdat);
 if (f->IsZombie()){
  cout << "Cannot open file: " << fdat << endl;
  return;
 }
  
 std::cout << " Input file = " << fdat << std::endl;
  
 //---- Histograms
 TH2F *hHS_HS2_HS1_X  = new TH2F("hHS_HS2_HS1_X", "Hodoscope 2 vs Hodoscope 1 X", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_HS2_HS1_Y  = new TH2F("hHS_HS2_HS1_Y", "Hodoscope 2 vs Hodoscope 1 Y", 64, 0, 64, 64, 0, 64);

 TH2F *hHS_HS2_HS1_numEntries_X  = new TH2F("hHS_HS2_HS1_numEntries_X", "Hodoscope 2 vs Hodoscope 1 X numEntries", 21, -1, 20, 21, -1, 20);
 TH2F *hHS_HS2_HS1_numEntries_Y  = new TH2F("hHS_HS2_HS1_numEntries_Y", "Hodoscope 2 vs Hodoscope 1 Y numEntries", 21, -1, 20, 21, -1, 20);
 
 //---- setup histograms
 
 //---- fill histograms
 TBEvent *event = new TBEvent();
 TTree *t1041 = (TTree*)f->Get("t1041");
 t1041->SetBranchAddress("tbevent",&event);
 
 Int_t start=0; Int_t end=t1041->GetEntries();
 
 std::cout << " analyzing " << end - start << " events " << std::endl;

 int nEntries = 0;
 for (Int_t i = start; i<end; i++) {
  t1041->GetEntry(i);
    
  Hodoscope hsch = event->GetHSChan();
  
  std::map<std::pair<int,int>, int > fibers = hsch.GetFibers();
  
  std::pair<int,int> fibers_mappairY1;
  fibers_mappairY1.first  = hodoY1;
  std::pair<int,int> fibers_mappairY2;
  fibers_mappairY2.first  = hodoY2;
  int numEntries_Y_1 = 0;
  int numEntries_Y_2 = 0;
  for(int iBinY1=0;iBinY1<64;iBinY1++){
   fibers_mappairY1.second = iBinY1;
   int valueY1 = fibers[fibers_mappairY1];
   if (valueY1 != 0) {
    numEntries_Y_1++;
    for(int iBinY2=0;iBinY2<64;iBinY2++){
     fibers_mappairY2.second = iBinY2;
     int valueY2 = fibers[fibers_mappairY2];
     if (valueY2 != 0) {
      hHS_HS2_HS1_Y->Fill(iBinY1,iBinY2);
     }
    }
   }
  }
  
  for(int iBinY2=0;iBinY2<64;iBinY2++){
   fibers_mappairY2.second = iBinY2;
   int valueY2 = fibers[fibers_mappairY2];
   if (valueY2 != 0) {
    numEntries_Y_2++;
   }
  }   
  
  hHS_HS2_HS1_numEntries_Y->Fill(numEntries_Y_1,numEntries_Y_2);
  hHS_HS2_HS1_numEntries_Y->Fill(-1,numEntries_Y_2);
  hHS_HS2_HS1_numEntries_Y->Fill(numEntries_Y_1,-1);
  
  
  
  std::pair<int,int> fibers_mappairX1;
  fibers_mappairX1.first  = hodoX1;
  std::pair<int,int> fibers_mappairX2;
  fibers_mappairX2.first  = hodoX2;
  int numEntries_X_1 = 0;
  int numEntries_X_2 = 0;
  for(int iBinX1=0;iBinX1<64;iBinX1++){
   fibers_mappairX1.second = iBinX1;
   int valueX1 = fibers[fibers_mappairX1];
   if (valueX1 != 0) {
    numEntries_X_1++;
    for(int iBinX2=0;iBinX2<64;iBinX2++){
     fibers_mappairX2.second = iBinX2;
     int valueX2 = fibers[fibers_mappairX2];
     if (valueX2 != 0) {
      hHS_HS2_HS1_X->Fill(iBinX1,iBinX2);
      nEntries++;
     }
    }
   }
  }
  
  for(int iBinX2=0;iBinX2<64;iBinX2++){
   fibers_mappairX2.second = iBinX2;
   int valueX2 = fibers[fibers_mappairX2];
   if (valueX2 != 0) {
    numEntries_X_2++;
   }
  }
  
  hHS_HS2_HS1_numEntries_X->Fill(numEntries_X_1,numEntries_X_2);
  hHS_HS2_HS1_numEntries_X->Fill(-1,numEntries_X_2);
  hHS_HS2_HS1_numEntries_X->Fill(numEntries_X_1,-1);
  
 }
 std::cout << " number of entries in histogram = " << nEntries << std::endl;
 
 TString canvasName = Form ("cc_HS_%s",fdat.Data());
 TString canvasNameHR = Form ("Hodoscope [%s]",fdat.Data()); 
 
 TCanvas *c1 = new TCanvas(canvasName.Data(),canvasNameHR.Data(),800,800);
 c1->Divide(2,2);
 
 c1->cd(1)->SetGrid();
 hHS_HS2_HS1_X->Draw("colz");
 c1->cd(2)->SetGrid();
 hHS_HS2_HS1_Y->Draw("colz");

 c1->cd(3)->SetGrid();
 hHS_HS2_HS1_numEntries_X->Draw("colz");
 c1->cd(4)->SetGrid();
 hHS_HS2_HS1_numEntries_Y->Draw("colz");
 
 
 
}

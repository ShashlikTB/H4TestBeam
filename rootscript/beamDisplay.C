#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBEvent.h"
#include "Mapper.h"

const int MAXADC=4095;



#define hodoX1 0
#define hodoY1 1
#define hodoX2 2
#define hodoY2 3

#include "plotTools.h"

// inputs data file and event in file to display (default is to integrate all)
void beamDisplay(TString fdat, int ndisplay=-1){
 
 gStyle->SetOptStat(0);
 
 TFile *f = new TFile(fdat);
 if (f->IsZombie()){
  cout << "Cannot open file: " << fdat << endl;
  return;
 }
 
 Bool_t singleEvent=ndisplay>=0;
 
 //---- Histograms
 TH2F *hWC   = new TH2F("hWC","Wire chambers; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_1  = new TH2F("hHS_1", "Hodoscope 1 ; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_2  = new TH2F("hHS_2", "Hodoscope 2 ; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_Y1 = new TH2F("hHS_Y1","Hodoscope Y1; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_Y2 = new TH2F("hHS_Y2","Hodoscope Y2; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_X1 = new TH2F("hHS_X1","Hodoscope X1; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);
 TH2F *hHS_X2 = new TH2F("hHS_X2","Hodoscope X2; X [mm]; Y [mm]", 64, 0, 64, 64, 0, 64);

 TH1F *hHS_Y1_1D = new TH1F("hHS_Y1_1D","Hodoscope Y1; Y [mm]", 64, 0, 64);
 TH1F *hHS_Y2_1D = new TH1F("hHS_Y2_1D","Hodoscope Y2; Y [mm]", 64, 0, 64);
 TH1F *hHS_X1_1D = new TH1F("hHS_X1_1D","Hodoscope X1; X [mm]", 64, 0, 64);
 TH1F *hHS_X2_1D = new TH1F("hHS_X2_1D","Hodoscope X2; X [mm]", 64, 0, 64);
 
 SetupHisto(*hHS_Y1_1D);
 SetupHisto(*hHS_Y2_1D);
 SetupHisto(*hHS_X1_1D);
 SetupHisto(*hHS_X2_1D);
 
 
 //---- setup histograms
 //   hHS_Y1->SetMinimum(-1);
 
 //---- fill histograms
 TBEvent *event = new TBEvent();
 TTree *t1041 = (TTree*)f->Get("t1041");
 TBranch *bevent = t1041->GetBranch("tbevent");
 bevent->SetAddress(&event);
 
 Int_t start=0; Int_t end=t1041->GetEntries();
 
 
 if (singleEvent && ndisplay<t1041->GetEntries() ) {
  start = ndisplay;
  end = ndisplay+1;
 }
 
 
 int nEntries = 0;
 for (Int_t i = start; i<end; i++) {
  t1041->GetEntry(i);
  Hodoscope hsch = event->GetHSChan();
//   hsch.Dump();
  std::map<std::pair<int,int>, int > fibers = hsch.GetFibers();
  
  std::pair<int,int> fibers_mappairY1;
  fibers_mappairY1.first  = hodoY1;
  std::pair<int,int> fibers_mappairY2;
  fibers_mappairY2.first  = hodoY2;
  for(int iBinY=0;iBinY<64;iBinY++){
   fibers_mappairY1.second = iBinY;
   fibers_mappairY2.second = iBinY;
   int valueY1 = fibers[fibers_mappairY1];
   int valueY2 = fibers[fibers_mappairY2];
   if (valueY1 != 0) {
    for (int iBinX=0; iBinX<64; iBinX++) {
     hHS_Y1->Fill(iBinX,iBinY);
    }
    hHS_Y1_1D->Fill(iBinY);
   }
   if (valueY2 != 0) {
    for (int iBinX=0; iBinX<64; iBinX++) {
     hHS_Y2->Fill(iBinX,iBinY);
    }
    hHS_Y2_1D->Fill(iBinY);
   }
  }
  
  
  std::pair<int,int> fibers_mappairX1;
  fibers_mappairX1.first  = hodoX1;
  std::pair<int,int> fibers_mappairX2;
  fibers_mappairX2.first  = hodoX2;
  for(int iBinX=0;iBinX<64;iBinX++){
   fibers_mappairX1.second = iBinX;
   fibers_mappairX2.second = iBinX;
   int valueX1 = fibers[fibers_mappairX1];
   int valueX2 = fibers[fibers_mappairX2];
   if (valueX1 != 0) {
    for (int iBinY=0; iBinY<64; iBinY++) {
     hHS_X1->Fill(iBinX,iBinY);
    }
    hHS_X1_1D->Fill(iBinX);
   }
   if (valueX2 != 0) {
    for (int iBinY=0; iBinY<64; iBinY++) {
     hHS_X2->Fill(iBinX,iBinY);
    }
    hHS_X2_1D->Fill(iBinX);
   }
  }
  
  
  
  
  
  
  
  for (int iBinX=0; iBinX<64; iBinX++) {
   for(int iBinY=0;iBinY<64;iBinY++){
    std::pair<int,int> fibers_mappairY1;
    fibers_mappairY1.first  = hodoY1;
    std::pair<int,int> fibers_mappairX1;
    fibers_mappairX1.first  = hodoX1;
    
    fibers_mappairY1.second = iBinY;
    fibers_mappairX1.second = iBinX;
    int valueY1 = fibers[fibers_mappairY1];
    int valueX1 = fibers[fibers_mappairX1];
    if (valueY1 != 0 && valueX1 != 0) {
     hHS_1->Fill(iBinX,iBinY);
    }
   }
  }
  
  for (int iBinX=0; iBinX<64; iBinX++) {
   for(int iBinY=0;iBinY<64;iBinY++){
    std::pair<int,int> fibers_mappairY2;
    fibers_mappairY2.first  = hodoY2;
    std::pair<int,int> fibers_mappairX2;
    fibers_mappairX2.first  = hodoX2;
    
    fibers_mappairY2.second = iBinY;
    fibers_mappairX2.second = iBinX;
    int valueY2 = fibers[fibers_mappairY2];
    int valueX2 = fibers[fibers_mappairX2];
    if (valueY2 != 0 && valueX2 != 0) {
     hHS_2->Fill(iBinX,iBinY);
    }
   }
  }
  
  
  nEntries++;
 }
 
 TCanvas *c1=new TCanvas("c1","Hodoscopes",800,800);
 c1->Divide(2,2);
 
 c1->cd(1)->SetGrid();
 hHS_Y1->Draw("colz");
 c1->cd(2)->SetGrid();
 hHS_Y2->Draw("colz");
 c1->cd(3)->SetGrid();
 hHS_X1->Draw("colz");
 c1->cd(4)->SetGrid();
 hHS_X2->Draw("colz");

 
 TCanvas *c1_1D = new TCanvas("c1_1D","Hodoscopes",800,800);
 c1_1D->Divide(2,2);
 
 c1_1D->cd(1)->SetGrid();
 hHS_Y1_1D->Draw();
 c1_1D->cd(2)->SetGrid();
 hHS_Y2_1D->Draw();
 c1_1D->cd(3)->SetGrid();
 hHS_X1_1D->Draw();
 c1_1D->cd(4)->SetGrid();
 hHS_X2_1D->Draw();
 c1_1D->SaveAs("plot/hs_1D.png");
 
 
 TCanvas *c2=new TCanvas("c2","Hodoscopes",800,400);
 c2->Divide(2,1);
 
 c2->cd(1)->SetGrid();
 hHS_1->Draw("colz");
 c2->cd(2)->SetGrid();
 hHS_2->Draw("colz");
 c2->SaveAs("plot/hs.png");
 

 
 //   return;   // skip timing
 
 
 
 //   TCanvas * c2 = new TCanvas("c2", "Average Peak Timing", 800, 800);
 //   c2->Divide(2, 2);
 // 
 //   c2->cd(1);
 //   hModD_time->Draw("colz");
 //   c2->cd(2);
 //   hModU_time->Draw("colz");
 //   c2->cd(3)->SetGrid();
 //   hChanD_time->Draw("colz");
 //   c2->cd(4)->SetGrid();
 //   hChanU_time->Draw("colz");
 //   //  drawChannelMap(c2);
 //   c2->Update();
 
}

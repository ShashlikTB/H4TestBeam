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
void beamPosition(TString fdat, int firstTime = 1){
 
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
  
 //---- Histograms
 TH2F *hHS_Shashlik_X_HS1_SHUp  = new TH2F("hHS_Shashlik_X_HS1_SHUp", "Hodoscope 1 vs Shashlik X Up", 128, -64, 64, 64, 0, 64);
 TH2F *hHS_Shashlik_Y_HS1_SHUp  = new TH2F("hHS_Shashlik_Y_HS1_SHUp", "Hodoscope 1 vs Shashlik Y Up", 128, -64, 64, 64, 0, 64);

 TH2F *hHS_Shashlik_X_HS2_SHUp  = new TH2F("hHS_Shashlik_X_HS2_SHUp", "Hodoscope 2 vs Shashlik X Up", 128, -64, 64, 64, 0, 64);
 TH2F *hHS_Shashlik_Y_HS2_SHUp  = new TH2F("hHS_Shashlik_Y_HS2_SHUp", "Hodoscope 2 vs Shashlik Y Up", 128, -64, 64, 64, 0, 64);

 TH2F *hHS_Shashlik_X_HS1_SHDo  = new TH2F("hHS_Shashlik_X_HS1_SHDo", "Hodoscope 1 vs Shashlik X Do", 128, -64, 64, 64, 0, 64);
 TH2F *hHS_Shashlik_Y_HS1_SHDo  = new TH2F("hHS_Shashlik_Y_HS1_SHDo", "Hodoscope 1 vs Shashlik Y Do", 128, -64, 64, 64, 0, 64);
 
 TH2F *hHS_Shashlik_X_HS2_SHDo  = new TH2F("hHS_Shashlik_X_HS2_SHDo", "Hodoscope 2 vs Shashlik X Do", 128, -64, 64, 64, 0, 64);
 TH2F *hHS_Shashlik_Y_HS2_SHDo  = new TH2F("hHS_Shashlik_Y_HS2_SHDo", "Hodoscope 2 vs Shashlik Y Do", 128, -64, 64, 64, 0, 64);
 
 
 hHS_Shashlik_X_HS1_SHUp->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_X_HS1_SHUp->GetYaxis()->SetTitle("Hodoscope");
 hHS_Shashlik_X_HS2_SHUp->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_X_HS2_SHUp->GetYaxis()->SetTitle("Hodoscope");
 hHS_Shashlik_X_HS1_SHDo->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_X_HS1_SHDo->GetYaxis()->SetTitle("Hodoscope");
 hHS_Shashlik_X_HS2_SHDo->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_X_HS2_SHDo->GetYaxis()->SetTitle("Hodoscope");
 
 hHS_Shashlik_Y_HS1_SHUp->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_Y_HS1_SHUp->GetYaxis()->SetTitle("Hodoscope");
 hHS_Shashlik_Y_HS2_SHUp->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_Y_HS2_SHUp->GetYaxis()->SetTitle("Hodoscope");
 hHS_Shashlik_Y_HS1_SHDo->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_Y_HS1_SHDo->GetYaxis()->SetTitle("Hodoscope");
 hHS_Shashlik_Y_HS2_SHDo->GetXaxis()->SetTitle("Shashlik");
 hHS_Shashlik_Y_HS2_SHDo->GetYaxis()->SetTitle("Hodoscope");
 
 
 //---- setup histograms
 
 //---- fill histograms
 TBEvent *event = new TBEvent();
 TTree *t1041 = (TTree*)f->Get("t1041");
 t1041->SetBranchAddress("tbevent",&event);
 
 Int_t start=0; Int_t end=t1041->GetEntries();
 

 
 bool haverechits=false;
 std::vector<TBRecHit> *rechits=0;
 if(t1041->GetListOfBranches()->FindObject("tbrechits")) {
  std::cout << " found rechits " << std::endl;
  t1041->SetBranchAddress("tbrechits",&rechits);
  haverechits = true;
 }
 
 Mapper *mapper = Mapper::Instance();
 
 
 
 int nEntries = 0;
 for (Int_t i = start; i<end; i++) {
  t1041->GetEntry(i);
  
  if (i==0) mapper->SetEpoch(event->GetTimeStamp());
  
  float m_u_maxADC = 0;
  float m_u_maxX = -99;
  float m_u_maxY = -99;
  float m_d_maxADC = 0;
  float m_d_maxX = -99;
  float m_d_maxY = -99;
  float c_u_maxADC = 0;
  float c_u_maxX = -99;
  float c_u_maxY = -99;
  float c_d_maxADC = 0;
  float c_d_maxX = -99;
  float c_d_maxY = -99;
  
  for (Int_t j = 0; j < event->NPadeChan(); j++){
   if (haverechits && j>=(int)rechits->size()) break;
   
   double ped,sig, max, maxTime;
   int channelID;
   
   if (haverechits){
    TBRecHit &hit=rechits->at(j);
    ped=hit.Pedestal();
    sig=hit.NoiseRMS();
    max=hit.AMax();
    maxTime=hit.TRise();
    channelID=hit.GetChannelID();
   }
   else{
    PadeChannel pch = event->GetPadeChan(j);
    pch.GetPedestal(ped,sig);
    max = pch.GetMax()-ped;
    maxTime = pch.GetPeak();
    channelID=pch.GetChannelID();   // boardID*100+channelNum in PADE
   }
   
   if (max<0.1) continue;
   
   int moduleID,fiberID;
   mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs
   
      
   double x,y;
   mapper->ModuleXY(moduleID,x,y);
   if (moduleID<0) {
    if(max > m_u_maxADC){
     m_u_maxADC = max;
     m_u_maxX = x;
     m_u_maxY = y;
    }
   }
   else {
    if(max > m_d_maxADC){
     m_d_maxADC = max;
     m_d_maxX = x;
     m_d_maxY = y;
    }
   }
   
   mapper->FiberXY(fiberID, x, y);
   if (moduleID<0) {
    if(max > c_u_maxADC){
     c_u_maxADC = max;
     c_u_maxX = x;
     c_u_maxY = y;
    }
   }
   else {
    if(max > c_d_maxADC){
     c_d_maxADC = max;
     c_d_maxX = x;
     c_d_maxY = y;
    }
   }
   
  }
  
  
  
  Hodoscope hsch = event->GetHSChan();
  
  std::map<std::pair<int,int>, int > fibers = hsch.GetFibers();
//   typedef std::map<std::pair<int,int>, int >::iterator it_type;
  
  std::pair<int,int> fibers_mappairY1;
  fibers_mappairY1.first  = hodoY1;
  std::pair<int,int> fibers_mappairY2;
  fibers_mappairY2.first  = hodoY2;
  for(int iBinY=0;iBinY<64;iBinY++){
   fibers_mappairY1.second = iBinY;
   fibers_mappairY2.second = iBinY;
   
//    std::cout << " here! " << std::endl;
//    it_type iterator = fibers.begin();
//    std::cout << " begin " << std::endl;
//    for(; iterator != fibers.end(); iterator++) {
//     std::cout << " ciclo ... " << std::endl;
//     if (iterator.first == fibers_mappairY1) {
//      valueY1 = iterator.second;
//     }
//     if (iterator.first == fibers_mappairY2) {
//      valueY2 = iterator.second;
//     }
//    }
//    std::cout << " there! " << std::endl;
   
   int valueY1 = fibers[fibers_mappairY1];
   int valueY2 = fibers[fibers_mappairY2];
   
   if (valueY1 != 0) {
    hHS_Shashlik_Y_HS1_SHUp->Fill(c_u_maxY,iBinY);
    hHS_Shashlik_Y_HS1_SHDo->Fill(c_d_maxY,iBinY);
   }
   if (valueY2 != 0) {
    hHS_Shashlik_Y_HS2_SHUp->Fill(c_u_maxY,iBinY);
    hHS_Shashlik_Y_HS2_SHDo->Fill(c_d_maxY,iBinY);
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
     hHS_Shashlik_X_HS1_SHUp->Fill(c_u_maxX,iBinX);
     hHS_Shashlik_X_HS1_SHDo->Fill(c_d_maxX,iBinX);
   }
   if (valueX2 != 0) {
     hHS_Shashlik_X_HS2_SHUp->Fill(c_u_maxX,iBinX);
     hHS_Shashlik_X_HS2_SHDo->Fill(c_d_maxX,iBinX);
   }
  }
   
  nEntries++;
 }
 
 
 TString canvasName = Form ("cc_HS1_%s",fdat.Data());
 TString canvasNameHR = Form ("Hodoscope 1 [%s]",fdat.Data());
 
 
 TCanvas *c1 = new TCanvas(canvasName.Data(),canvasNameHR.Data(),800,800);
 c1->Divide(2,2);
 
 c1->cd(1)->SetGrid();
 hHS_Shashlik_X_HS1_SHUp->Draw("colz");
 c1->cd(2)->SetGrid();
 hHS_Shashlik_Y_HS1_SHUp->Draw("colz");
 c1->cd(3)->SetGrid();
 hHS_Shashlik_X_HS1_SHDo->Draw("colz");
 c1->cd(4)->SetGrid();
 hHS_Shashlik_Y_HS1_SHDo->Draw("colz");

 canvasName = Form ("cc_HS2_%s",fdat.Data());
 canvasNameHR = Form ("Hodoscope 2 [%s]",fdat.Data());
 
 
 TCanvas *c2 = new TCanvas(canvasName.Data(),canvasNameHR.Data(),800,800);
 c2->Divide(2,2);
 
 c2->cd(1)->SetGrid();
 hHS_Shashlik_X_HS2_SHUp->Draw("colz");
 c2->cd(2)->SetGrid();
 hHS_Shashlik_Y_HS2_SHUp->Draw("colz");
 c2->cd(3)->SetGrid();
 hHS_Shashlik_X_HS2_SHDo->Draw("colz");
 c2->cd(4)->SetGrid();
 hHS_Shashlik_Y_HS2_SHDo->Draw("colz");
 
 
}

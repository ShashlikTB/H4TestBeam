#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"

// #include "include/TBEvent.h"
// #include "include/TBRecHit.h"
// #include "include/Mapper.h"


const float FIBER_OFFSET_X=3.5; ///< x offset of fiber placements from center of module in mm (not acurate!)
const float FIBER_OFFSET_Y=3.5; ///< y offset of fiber placements from center of module in mm (not acurate!)
const float MIN_EDGE_X=-28;
const float MIN_EDGE_Y=-28;
const float MAX_EDGE_X=28;
const float MAX_EDGE_Y=28;


const int MAXADC=4095;

// inputs data file and event in file to display (default is to integrate all)
void calOccupancy(TString fdat, int firstTime = 1){

 if (firstTime) {
  gROOT->ProcessLine("#include \"include/TBEvent.h\"");
  gROOT->ProcessLine("#include \"include/TBRecHit.h\"");
  gROOT->ProcessLine("#include \"include/Mapper.h\"");
 }
 
  gStyle->SetOptStat(0);

  TFile *f = new TFile(fdat);
  if (f->IsZombie()){
    std::cout << "Cannot open file: " << fdat <<  std::endl;
    return;
  }
  std::cout << "File: " << fdat <<  std::endl;
  
  Mapper *mapper = Mapper::Instance();

  // Histograms of ADC counts
  TH2F *hModU=new TH2F("hModU","Modules UpSteam RO;X [mm]; Y [mm]",4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hModD=new TH2F("hModD","Modules DownStream RO;X [mm]; Y [mm]",4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanU=new TH2F("hChanU","Channels UpStream RO;X [mm]; Y [mm]",8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanD=new TH2F("hChanD","Channels DownStream RO;X [mm]; Y [mm]",8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);


  hModU->SetMinimum(0);
  hModD->SetMinimum(0);
  hChanU->SetMinimum(0);
  hChanD->SetMinimum(0);
  hChanU->GetXaxis()->SetNdivisions(8,0);
  hChanU->GetYaxis()->SetNdivisions(8,0);
  hChanD->GetXaxis()->SetNdivisions(8,0);
  hChanD->GetYaxis()->SetNdivisions(8,0);

  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*) f->Get("t1041");
  t1041->SetBranchAddress("tbevent",&event);
  
  bool haverechits=false;
  vector<TBRecHit> *rechits=0;
  if(t1041->GetListOfBranches()->FindObject("tbrechits")) {
    std::cout <<"found rechits"<< std::endl;
    t1041->SetBranchAddress("tbrechits",&rechits);
    haverechits = true;
  }

  Int_t start=0;
  Int_t end=t1041->GetEntries();
  
  int nEntries=0;
  for (Int_t i=start; i<end; i++) {
    t1041->GetEntry(i);
    if (i==0) mapper->SetEpoch(event->GetTimeStamp());

    float m_u_maxADC = 0;
    float m_d_maxADC = 0;

    for (Int_t j = 0; j < event->NPadeChan(); j++){
      if (haverechits && j>=(int)rechits->size()) break;

      double ped,sig, max, maxTime;
      int channelID;

      if (haverechits) {
        TBRecHit &hit=rechits->at(j);
        ped=hit.Pedestal();
        sig=hit.NoiseRMS();
        max=hit.AMax();
        maxTime=hit.TRise();
        channelID=hit.GetChannelID();
      }
      else {
        PadeChannel pch = event->GetPadeChan(j);
        pch.GetPedestal(ped,sig);
        max = pch.GetMax()-ped;
        maxTime = pch.GetPeak();
        channelID=pch.GetChannelID();   // boardID*100+channelNum in PADE
      }

      if (max<0.1) continue;

      int moduleID,fiberID;
      mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs


//       std::cout << "Printint Fiber ID and Module ID: ##########" <<  std::endl;
//       std::cout << "Module ID: " << moduleID <<  std::endl;
//       std::cout << "Fiber ID: " << fiberID <<  std::endl;
//       std::cout << "####################" <<  std::endl;

      double x,y;
      mapper->ModuleXY(moduleID,x,y);
      if (moduleID<0) {
        hModU->Fill(x, y, max);
      }
      else {
        hModD->Fill(x, y, max);
      }
      
      mapper->FiberXY(fiberID, x, y);
      if (moduleID<0) {
        hChanU->Fill(x, y, max);
      }
      else {
        hChanD->Fill(x, y, max);
      }

    }
  }

  // fetch module and channel maps
  TH2I *hmModU = new TH2I();
  mapper->GetModuleMap(hmModU,-1);
  TH2I *hmModD = new TH2I();
  mapper->GetModuleMap(hmModD,1);
  TH2I *hmChanU = new TH2I();
  mapper->GetChannelMap(hmChanU,-1);
  TH2I *hmChanD = new TH2I();
  mapper->GetChannelMap(hmChanD,1);


  TCanvas *c1 = new TCanvas("c1","Occupancy",800,800);
  c1->Divide(2,2);

  c1->cd(1);
  hModD->Draw("colz");
  hmModD->Draw("text same");
  c1->cd(2);
  hModU->Draw("colz");
  hmModU->Draw("text same");
  c1->cd(3)->SetGrid();
  hChanD->Draw("colz");
  hmChanD->Draw("text same");
  c1->cd(4)->SetGrid();
  hChanU->Draw("colz");
  hmChanU->Draw("text same");


}




#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBEvent.h"
#include "TBRecHit.h"
#include "Mapper.h"
#include "TROOT.h"
#include <cmath>

const int MAXADC=4095;
// inputs data file and event in file to display (default is to integrate all)

void calCrystalDepo(TTree *t1041, int ThisModule = -99, bool IsBatch =0){
  if(IsBatch) gROOT->SetBatch();

  TString ThisModuleString = TString::Itoa(ThisModule,10);
  int UpOrDown = ThisModule/abs(ThisModule);
  float Scale = 1.;
  if(UpOrDown == -1) Scale = 0.5;
  cout << "Creating energy distributions of events with maximum deposition in module: " << ThisModuleString << endl;
//Define Histograms

//This histogram is a tool
  TH1F* MatrixDepo = new TH1F("MatrixDepo", "Deposition on Matrix", 68, -17, 17);

  TH1F* fracDep = new TH1F("fracDep", "Highest Amplitude Crystal/Total Deposition", 100, 0, 1.2);
  TH1F* MaxBin = new TH1F("MaxBin", "Deposition of Highest Amplitude Crystal", 100,400*Scale,7000*Scale);
  TH1F* Integral = new TH1F("Integral", "Integral", 100,4000*Scale,13000*Scale);
  TH1F* SigNoise = new TH1F("SigNoise", "Signa/Pedestal", 100, 0, 2.);

//

  gStyle->SetOptStat(0);
  

  Mapper *mapper=Mapper::Instance();

  TBEvent *event = new TBEvent();
  t1041->SetBranchAddress("tbevent", &event);
  bool haverechits=false;
  vector<TBRecHit> *rechits=0;

  if(t1041->GetListOfBranches()->FindObject("tbrechits")) {
    cout <<"found rechits"<<endl;
    t1041->SetBranchAddress("tbrechits",&rechits);
    haverechits=true;
  }

  Int_t start=0; Int_t end=t1041->GetEntries();  
  
  int nEntries=0;
//  end = 10;
  for (Int_t i=start; i<end; i++) {
    t1041->GetEntry(i);
    if (i==0) mapper->SetEpoch(event->GetTimeStamp());
    if(haverechits && rechits->size() < 1) continue;

    MatrixDepo->Reset();
    float maxDepoModuleID = -99;
    float maxDepo = 0;
    float secondMaxDepo = 0;
    float secondMaxDepoModuleID = -99;
    float totdep = 0;
    float totalnoise = 0;
    float sumrms = 0;
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

      double x,y;
      int moduleID,fiberID;
      mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);
      mapper->ModuleXY(moduleID,x,y);

      MatrixDepo->Fill(moduleID, max);      
      if(UpOrDown > 0 && moduleID > 0)	totalnoise+=ped;
      if(UpOrDown < 0 && moduleID < 0)	totalnoise+=ped;
    }

    int minx = 0;
    int maxx = 0;
    if(UpOrDown > 0){
      minx = MatrixDepo->GetXaxis()->FindBin(0.);
      maxx = MatrixDepo->GetXaxis()->FindBin(17.);
      MatrixDepo->GetXaxis()->SetRange(minx, maxx);
    }
    if(UpOrDown < 0){
      minx = MatrixDepo->GetXaxis()->FindBin(-17.);
      maxx = MatrixDepo->GetXaxis()->FindBin(0.);	
      MatrixDepo->GetXaxis()->SetRange(minx, maxx);
    }

//    cout << "TOTAL NOISE: " << totalnoise << endl;

    float maxbin = MatrixDepo->GetBinContent(MatrixDepo->GetMaximumBin());
    float integral = MatrixDepo->Integral(minx,maxx);

    maxDepoModuleID = MatrixDepo->GetBinLowEdge(MatrixDepo->GetMaximumBin());

    if( abs(ThisModule) != 99 && maxDepoModuleID != ThisModule ) continue;
    if( abs(ThisModule) == 100 ){
	if( abs(maxDepoModuleID) != 6 &&
	    abs(maxDepoModuleID) != 7 &&
	    abs(maxDepoModuleID) != 10 &&
	    abs(maxDepoModuleID) != 11) continue;
    }

    
//    if(sumrms/totdep > 0.3) continue;
//      if( maxDepo/totdep < 0.2 ||  maxDepo/totdep > 0.5) continue;
      if( maxbin/integral < .55) continue;
//      if( totdep/totalnoise < 1.) continue;
//      if( totdep < 950) continue;
//    if(sumrms/totdep < 0.3) cout << totdep << endl;




    MaxBin->Fill(maxbin);
    Integral->Fill(integral);
    fracDep->Fill(maxbin/integral);
//    cout << maxbin/integral;
//    SigNoise->Fill(integral/totalnoise);
//    cout << maxbin << endl;

    nEntries++;
  }

  TString FileFig;
  TCanvas *c5=new TCanvas("c5","Average Peak Height",800,800);
  c5->SetGrid();
  fracDep->Draw();
  FileFig = "energy/R9_Module_"+ThisModuleString+".png";
  c5->SaveAs(FileFig);

  TCanvas *c7=new TCanvas("c7","MAXBIN",800,800);
  c7->SetGrid();
//  MaxBin->Fit("gaus", "","", 4100, 6000);
  MaxBin->Draw();
  FileFig = "energy/eMaxCrystal_Module_"+ThisModuleString+".png";
  c7->SaveAs(FileFig);

  TCanvas *c8=new TCanvas("c8","Integral",800,800);
  c8->SetGrid();
//  TF1* fit = new TF1("fit","p0*exp(-(0.5)*((x-760)/p1)^2)",0, 3000);
//  fit->SetParameters(500, 50);
  Integral->Fit("gaus");//, "", "", 3000, 4000);
  gStyle->SetOptFit();
  Integral->Draw();
  FileFig = "energy/eMatrix_Module_"+ThisModuleString+".png";
  c8->SaveAs(FileFig);

  TCanvas *c9=new TCanvas("c9","Integral",800,800);
  c9->SetGrid();
  SigNoise->Draw();
  FileFig = "energy/SignalToNoise_Module"+ThisModuleString+".png";
  c9->SaveAs(FileFig);


}

// Created 8/25/2014 B.Hirosky: Initial release
// Basic DQM plotter 

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <vector>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TMath.h>
#include <TString.h>
#include <TStyle.h>
#include <iostream>
// #include "TBEvent.h"
// #include "TBRecHit.h"
// #include "Mapper.h"
//#include "calConstants.h"

using std::cout;
using std::endl;
using std::vector;
using TMath::Min;


TBEvent *event = new TBEvent();
//vector<TBTrack> *tracks=0;
vector<TBRecHit> *rechits=0;


void calo_DQM(TTree *tree, TString file){
  TString canvasName = Form ("cc_DQM_%s",file.Data());
  TCanvas *cCal=new TCanvas(canvasName.Data(),"CaloDQM: Slopes, Projections",1500,800);
  cCal->Divide(4,2);
  TH2F *hNoise=new TH2F("hNoise","Noise vs Channel;channel index;ADC counts",128,0,128,20,0,2);
  TH2F *hAmp=new TH2F("hAmp","log(Amplitude) vs Channel;channel index;log(ADC counts)",128,0,128,40,0,3.5);
  TH2F *hChi2=new TH2F("hChi2","log(Chi2) vs Channel;channel number;log(Chi2)",128,0,128,40,0,6);
  TH2F *hNdof=new TH2F("hNdof","Ndof vs Channel;channel number;NDOF fit",128,0,128,40,0,40);
  TH2F *hChi2U=new TH2F("hChi2U","Upstream: log(Chi2) vs Amplitude;Amplitude;log(Chi2)",128,0,4096,40,0,6);
  TH2F *hChi2D=new TH2F("hChi2D","Downstream: log(Chi2) vs Amplitude;Amplitude;log(Chi2)",128,0,4096,40,0,6);
  
  hAmp->SetStats(0);
  hNoise->SetStats(0);
  hChi2->SetStats(0);
  hNdof->SetStats(0);

  ULong_t ts=0;
  for (Int_t i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);
    for (unsigned c=0;c<rechits->size(); c++){
      if (c==0) ts=event->GetTimeStamp();
      TBRecHit &hit=rechits->at(c);
      double amax=hit.AMax();
      double chi2=hit.Chi2();
      if (amax>0) hAmp->Fill( hit.ChannelIndex(), TMath::Log10(amax) );
      if (chi2>0) hChi2->Fill( hit.ChannelIndex(), TMath::Log10(chi2) );
      hNoise->Fill( hit.ChannelIndex(), hit.NoiseRMS() );
      hNdof->Fill( hit.ChannelIndex(), hit.Ndof());
      if (hit.ChannelIndex()<64) hChi2U->Fill( amax, TMath::Log10(chi2) );
      else hChi2D->Fill( amax, TMath::Log10(chi2) );
    }
  }
  cCal->cd(1);
  hAmp->Draw("Colz");
  cCal->cd(2);
  hNoise->Draw("Colz");
  cCal->cd(3);
  hChi2U->Draw("Col");

  cCal->cd(5);
  hChi2->Draw("Colz");
  cCal->cd(6);
  hNdof->Draw("Colz");
  cCal->cd(7);
  hChi2D->Draw("Col");



  // add maping table
  Mapper *mapper=Mapper::Instance();
  mapper->SetEpoch(ts);

  // contiguous channel Indexs
  TH2I *hIdxU=new TH2I();
  mapper->GetChannelIdx(hIdxU,-1);
  TH2I *hIdxD=new TH2I();
  mapper->GetChannelIdx(hIdxD,1);
  cCal->cd(4);
  hIdxU->Draw("text");
  cCal->cd(8);
  hIdxD->Draw("text");
}


void calDQM(TString file="latest_reco.root", int firstTime = 1){
 
 if (firstTime) {
  gROOT->ProcessLine("#include \"include/TBEvent.h\"");
  gROOT->ProcessLine("#include \"include/TBRecHit.h\"");
  gROOT->ProcessLine("#include \"include/Mapper.h\"");
 }
 
 gROOT->SetStyle("Plain");
 gStyle->SetOptStat(0);
 
 TFile *f = new TFile(file);
 if (f->IsZombie()){
  cout << "Cannot open file: " << file << endl;
  return;
 }
 
 TTree *t1041 = (TTree*) f->Get("t1041");
 t1041->SetBranchAddress("tbevent",&event);
 //t1041->SetBranchAddress("tbtracks",&tracks);
 t1041->SetBranchAddress("tbrechits",&rechits);  
 
 cout << "Analyzing: " << file << endl;
 
 //  WC_DQM(f,t1041);
 //track_DQM(t1041);
 calo_DQM(t1041,file);
 return;
 
}


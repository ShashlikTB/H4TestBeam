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

#include <cmath>

const int MAXADC=4095;


// inputs data file and event in file to display (default is to integrate all)
void calCorrelator(TString fdat, int ndisplay=-1){

  bool DO_TIMING=false;  // more for experts
  bool DO_WEIGHTED=true;

  gStyle->SetOptStat(0);

  TFile *f = new TFile(fdat);
  if (f->IsZombie()){
    cout << "Cannot open file: " << fdat << endl;
    return;
  }
  
  Bool_t singleEvent=ndisplay>=0;
  Mapper *mapper=Mapper::Instance();

  // Histograms of ADC counts

  TH2F *hChanU=new TH2F("hChanU","Channels UpStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanD=new TH2F("hChanD","Channels DownStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *ChanCorrX=new TH2F("ChanCorrX", "Correlation in X Between Largest Deposition in Up and Downstream ROs;X Downstream [mm];X Upstream[mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_X,MAX_EDGE_X);
  TH2F *ChanCorrY=new TH2F("ChanCorrX", "Correlation in Y Between Largest Deposition in Up and Downstream ROs;Y Downstream [mm];Y Upstream[mm]",
			8,MIN_EDGE_Y,MAX_EDGE_Y,8,MIN_EDGE_Y,MAX_EDGE_Y);

  hChanU->SetMinimum(0);
  hChanD->SetMinimum(0);
  hChanU->GetXaxis()->SetNdivisions(8,0);
  hChanU->GetYaxis()->SetNdivisions(8,0);
  hChanD->GetXaxis()->SetNdivisions(8,0);
  hChanD->GetYaxis()->SetNdivisions(8,0);

  ChanCorrX->SetMinimum(0);
  ChanCorrX->GetXaxis()->SetNdivisions(8,0);
  ChanCorrX->GetYaxis()->SetNdivisions(8,0);
  ChanCorrY->SetMinimum(0);
  ChanCorrY->GetXaxis()->SetNdivisions(8,0);
  ChanCorrY->GetYaxis()->SetNdivisions(8,0);

  if (singleEvent){
    hChanU->SetMaximum(MAXADC);
    hChanU->SetMaximum(MAXADC);
    ChanCorrX->SetMaximum(MAXADC);
    ChanCorrY
->SetMaximum(MAXADC);
  }

  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*)f->Get("t1041");
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);
  bool haverechits=false;
  vector<TBRecHit> *rechits=0;
  if(t1041->GetListOfBranches()->FindObject("tbrechits")) {
    cout <<"found rechits"<<endl;
    t1041->SetBranchAddress("tbrechits",&rechits);
    haverechits=true;
  }

  Int_t start=0; Int_t end=t1041->GetEntries();
  
  if (singleEvent && ndisplay<t1041->GetEntries() ) {
    start=ndisplay;
    end=ndisplay+1;
  }
  
  int nEntries=0;
  
  for (Int_t i=start; i<end; i++) {

    t1041->GetEntry(i);
    if (i==0) mapper->SetEpoch(event->GetTimeStamp());
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
      if (!max) continue;
      int moduleID,fiberID;
      mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs
      double x,y;
      mapper->FiberXY(fiberID, x, y);
      if (moduleID<0) {
	if(max > c_u_maxADC){
	  c_u_maxADC = max;
	  c_u_maxX = x;
	  c_u_maxY = y;
	}
      } else{
	hChanD->Fill(x, y, max);
	if(max > c_d_maxADC){
	  c_d_maxADC = max;
	  c_d_maxX = x;
	  c_d_maxY = y;
	}
     }
    }
    if( c_u_maxX > -99 && c_u_maxY > -99 && c_u_maxADC > 0)
    { 
	hChanU->Fill(c_u_maxX, c_u_maxY); 
    }

    if( c_d_maxX > -99 && c_d_maxY > -99 && c_d_maxADC > 0)
    { 
	hChanD->Fill(c_d_maxX, c_d_maxY); 
    }
    if( c_u_maxADC > 0 && c_d_maxADC > 0){
	ChanCorrX->Fill(c_d_maxX, c_u_maxX);
	ChanCorrY->Fill(c_d_maxY, c_u_maxY);
    }

    nEntries++;
  }


  hChanD->Scale(1./nEntries);
  hChanU->Scale(1./nEntries);
  ChanCorrX->Scale(1./nEntries);
  ChanCorrY->Scale(1./nEntries);

  // fetch module and channel maps
  TH2I *hmChanU=new TH2I();
  mapper->GetChannelMap(hmChanU,-1);
  TH2I *hmChanD=new TH2I();
  mapper->GetChannelMap(hmChanD,1);


  TCanvas *c1=new TCanvas("c1","Average Peak Height",800,800);
  c1->Divide(2,2);

  c1->cd(1)->SetGrid();
  hChanD->Draw("colz");
  hmChanD->Draw("text same");
  c1->cd(2)->SetGrid();
  hChanU->Draw("colz");
  hmChanU->Draw("text same");
  c1->cd(3);
  ChanCorrX->Draw("colz");
  c1->cd(4);
  ChanCorrY->Draw("colz");
  c1->SaveAs("plot/correlation.png");


}

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


// inputs data file and event in file to display (default is to integrate all)
void calDisplay(TString fdat, int ndisplay=-1){

  gStyle->SetOptStat(0);

  TFile *f = new TFile(fdat);
  if (f->IsZombie()){
    cout << "Cannot open file: " << fdat << endl;
    return;
  }
  
  Bool_t singleEvent=ndisplay>=0;
  Mapper *mapper=Mapper::Instance();

  // Histograms of ADC counts
  TH2F *hModU=new TH2F("hModU","Modules UpSteam RO;X [mm]; Y [mm]",
		       4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hModD=new TH2F("hModD","Modules DownStream RO;X [mm]; Y [mm]",
		       4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanU=new TH2F("hChanU","Channels UpStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanD=new TH2F("hChanD","Channels DownStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);


  TH2F *hModUmax=new TH2F("hModUmax","Maximum Deposition Modules UpSteam RO;X [mm]; Y [mm]",
		       4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hModDmax=new TH2F("hModDmax","Maximum Deposition Modules DownStream RO;X [mm]; Y [mm]",
		       4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanUmax=new TH2F("hChanUmax","Maximum Deposition Channels UpStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanDmax=new TH2F("hChanDmax","Maximum Deposition Channels DownStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);


  TH2F *hModUmaxW=new TH2F("hModUmaxW","Weighter Maximum Deposition Modules UpSteam RO;X [mm]; Y [mm]",
		       4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hModDmaxW=new TH2F("hModDmaxW","Weighted Maximum Deposition Modules DownStream RO;X [mm]; Y [mm]",
		       4,MIN_EDGE_X,MAX_EDGE_X,4,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanUmaxW=new TH2F("hChanUmaxW","Weighted Maximum Deposition Channels UpStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);
  TH2F *hChanDmaxW=new TH2F("hChanDmaxW","Weighted Maximum Deposition Channels DownStream RO;X [mm]; Y [mm]",
			8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y);


  hModU->SetMinimum(0);
  hModD->SetMinimum(0);
  hChanU->SetMinimum(0);
  hChanD->SetMinimum(0);
  hChanU->GetXaxis()->SetNdivisions(8,0);
  hChanU->GetYaxis()->SetNdivisions(8,0);
  hChanD->GetXaxis()->SetNdivisions(8,0);
  hChanD->GetYaxis()->SetNdivisions(8,0);

  hModUmax->SetMinimum(0);
  hModDmax->SetMinimum(0);
  hChanUmax->SetMinimum(0);
  hChanDmax->SetMinimum(0);
  hChanUmax->GetXaxis()->SetNdivisions(8,0);
  hChanUmax->GetYaxis()->SetNdivisions(8,0);
  hChanDmax->GetXaxis()->SetNdivisions(8,0);
  hChanDmax->GetYaxis()->SetNdivisions(8,0);

  hModUmaxW->SetMinimum(0);
  hModDmaxW->SetMinimum(0);
  hChanUmaxW->SetMinimum(0);
  hChanDmaxW->SetMinimum(0);
  hChanUmaxW->GetXaxis()->SetNdivisions(8,0);
  hChanUmaxW->GetYaxis()->SetNdivisions(8,0);
  hChanDmaxW->GetXaxis()->SetNdivisions(8,0);
  hChanDmaxW->GetYaxis()->SetNdivisions(8,0);


  if (singleEvent){
    hModU->SetMaximum(MAXADC*4);
    hModD->SetMaximum(MAXADC*4);
    hChanU->SetMaximum(MAXADC);
    hChanU->SetMaximum(MAXADC);

    hModUmax->SetMaximum(MAXADC*4);
    hModDmax->SetMaximum(MAXADC*4);
    hChanUmax->SetMaximum(MAXADC);
    hChanUmax->SetMaximum(MAXADC);

    hModUmaxW->SetMaximum(MAXADC*4);
    hModDmaxW->SetMaximum(MAXADC*4);
    hChanUmaxW->SetMaximum(MAXADC);
    hChanUmaxW->SetMaximum(MAXADC);
  }
  
  // histograms of timing 
  TH2F * hModD_time = (TH2F*)hModU->Clone("hModD_time");
  TH2F * hModU_time = (TH2F*)hModU->Clone("hModU_time");
  TH2F * hChanD_time = (TH2F*)hModU->Clone("hChanD_time");
  TH2F * hChanU_time = (TH2F*)hModU->Clone("hChanU_time");

  TBEvent *event = new TBEvent();
  TTree *t1041 = (TTree*)f->Get("t1041");
  TBranch *bevent = t1041->GetBranch("tbevent");
  bevent->SetAddress(&event);

  Int_t start=0; Int_t end=t1041->GetEntries();
  

  if (singleEvent && ndisplay<t1041->GetEntries() ) {
    start=ndisplay;
    end=ndisplay+1;
  }
  
  int nEntries=0;
  for (Int_t i=start; i<end; i++) {
    t1041->GetEntry(i);

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
      PadeChannel pch = event->GetPadeChan(j);
      if (j==0) mapper->SetEpoch(pch.GetTimeStamp());
      double ped,sig;
      pch.GetPedestal(ped,sig);
      UShort_t max = pch.GetMax()-ped;
      Int_t maxTime = pch.GetPeak();
      if (max<5) continue;
      if (max>MAXADC) continue;    // skip channels with bad adc readings (should be RARE)
      //max-=pch.GetPedestal();

      int channelID=pch.GetChannelID();   // boardID*100+channelNum in PADE
      int moduleID,fiberID;
      mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs


//       cout << "Printint Fiber ID and Module ID: ##########" << endl;
//       cout << "Module ID: " << moduleID << endl;
//       cout << "Fiber ID: " << fiberID << endl;
//       cout << "####################" << endl;

      double x,y;
      mapper->ModuleXY(moduleID,x,y);
      if (moduleID<0) {
	hModU->Fill(x, y, max);
	hModU_time->Fill(x, y, maxTime);
	if(max > m_u_maxADC){
	  m_u_maxADC = max;
	  m_u_maxX = x;
	  m_u_maxY = y;
	}
      }
      else {
	hModD->Fill(x, y, max);
	hModD_time->Fill(x, y, maxTime);
	if(max > m_d_maxADC){
	  m_d_maxADC = max;
	  m_d_maxX = x;
	  m_d_maxY = y;
	}
      }
      
      mapper->FiberXY(fiberID, x, y);
      if (moduleID<0) {
	hChanU->Fill(x, y, max);
	hChanU_time->Fill(x, y, maxTime);
	if(max > c_u_maxADC){
	  c_u_maxADC = max;
	  c_u_maxX = x;
	  c_u_maxY = y;
	}
      }
      else {
	hChanD->Fill(x, y, max);
	hChanD_time->Fill(x, y, maxTime);
	if(max > c_d_maxADC){
	  c_d_maxADC = max;
	  c_d_maxX = x;
	  c_d_maxY = y;
	}
      }

    }

    if( m_u_maxX > -99 && m_u_maxY > -99 && m_u_maxADC > 0)
    { 
	hModUmax->Fill(m_u_maxX, m_u_maxY); 
	hModUmaxW->Fill(m_u_maxX, m_u_maxY, m_u_maxADC); 

    }

    if( m_d_maxX > -99 && m_d_maxY > -99 && m_d_maxADC > 0)
    { 
	hModDmax->Fill(m_d_maxX, m_d_maxY); 
	hModDmaxW->Fill(m_d_maxX, m_d_maxY, m_d_maxADC); 
    }

    if( c_u_maxX > -99 && c_u_maxY > -99 && c_u_maxADC > 0)
    {
	hChanUmax->Fill(c_u_maxX, c_u_maxY); 
	hChanUmaxW->Fill(c_u_maxX, c_u_maxY, c_u_maxADC); 

    }

    if( c_d_maxX > -99 && c_d_maxY > -99 && c_d_maxADC > 0)
    {
	hChanDmax->Fill(c_d_maxX, c_d_maxY);
	hChanDmaxW->Fill(c_d_maxX, c_d_maxY, c_d_maxADC);
    }
    

    nEntries++;
  }

  hModD->Scale(1./nEntries);
  hModU->Scale(1./nEntries);
  hChanD->Scale(1./nEntries);
  hChanU->Scale(1./nEntries);
  
  hModD_time->Scale(1./nEntries);
  hModU_time->Scale(1./nEntries);
  hChanD_time->Scale(1./nEntries);
  hChanU_time->Scale(1./nEntries);

  hModDmax->Scale(1./nEntries);
  hModUmax->Scale(1./nEntries);
  hChanDmax->Scale(1./nEntries);
  hChanUmax->Scale(1./nEntries);

  hModDmaxW->Scale(1./nEntries);
  hModUmaxW->Scale(1./nEntries);
  hChanDmaxW->Scale(1./nEntries);
  hChanUmaxW->Scale(1./nEntries);

  // fetch module and channel maps
  TH2I *hmModU=new TH2I();
  mapper->GetModuleMap(hmModU,-1);
  TH2I *hmModD=new TH2I();
  mapper->GetModuleMap(hmModD,1);
  TH2I *hmChanU=new TH2I();
  mapper->GetChannelMap(hmChanU,-1);
  TH2I *hmChanD=new TH2I();
  mapper->GetChannelMap(hmChanD,1);


  TCanvas *c1=new TCanvas("c1","Average Peak Height",800,800);
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
  c1->SaveAs("plot/cal_peak.png");
  
//   return;   // skip timing
//

  TCanvas * c2 = new TCanvas("c2", "Average Peak Timing", 800, 800);
  c2->Divide(2, 2);

  c2->cd(1);
  hModD_time->Draw("colz");
  c2->cd(2);
  hModU_time->Draw("colz");
  c2->cd(3)->SetGrid();
  hChanD_time->Draw("colz");
  c2->cd(4)->SetGrid();
  hChanU_time->Draw("colz");
  //  drawChannelMap(c2);
  c2->Update();
  c2->SaveAs("plot/cal_time.png");
  
  TCanvas *c3=new TCanvas("c3","Local of Maximum Deposition of Each Event",800,800);
  c3->Divide(2,2);

  c3->cd(1);
  hModDmax->Draw("colz");
  hmModD->Draw("text same");
  c3->cd(2);
  hModUmax->Draw("colz");
  hmModU->Draw("text same");
  c3->cd(3)->SetGrid();
  hChanDmax->Draw("colz");
  hmChanD->Draw("text same");
  c3->cd(4)->SetGrid();
  hChanUmax->Draw("colz");
  hmChanU->Draw("text same");
  c3->SaveAs("plot/cal_peak_max.png");

  TCanvas *c4=new TCanvas("c4","Weighted Maximum Peak Height of Each Event",800,800);
  c4->Divide(2,2);

  c4->cd(1);
  hModDmaxW->Draw("colz");
  hmModD->Draw("text same");
  c4->cd(2);
  hModUmaxW->Draw("colz");
  hmModU->Draw("text same");
  c4->cd(3)->SetGrid();
  hChanDmaxW->Draw("colz");
  hmChanD->Draw("text same");
  c4->cd(4)->SetGrid();
  hChanUmaxW->Draw("colz");
  hmChanU->Draw("text same");
  c4->SaveAs("plot/cal_peak_maxW.png");
}

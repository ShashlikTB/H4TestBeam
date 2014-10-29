{
 std::cout << std::endl << "-------------------";
 std::cout << std::endl << "Welcome to the ROOT";
 std::cout << std::endl << "-------------------" << std::endl;
 std::cout << std::endl;
 
 gROOT->Reset();
 gROOT->SetStyle("Plain");
 gStyle->SetPalette(1);
 gStyle->SetOptStat(111111);
 gStyle->SetOptFit(1111);
 
 
 #include "TROOT.h"
 #include "TStyle.h"
 #include "TColor.h"
 const Int_t __NRGBs = 5;
 //   const Int_t __NCont = 999;
 const Int_t __NCont = 76;
 Double_t __stops[__NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
 Double_t __red[__NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
 Double_t __green[__NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
 Double_t __blue[__NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
 
 //UInt_t Number = 5;
 //Double_t Red[5]   = { 0.00, 0.09, 0.18, 0.09, 0.00 };
 //Double_t Green[5] = { 0.01, 0.02, 0.39, 0.68, 0.97 };
 //Double_t Blue[5]  = { 0.17, 0.39, 0.62, 0.79, 0.97 };
 //Double_t Stops[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
 //UInt_t NColors = 999;
 //TColor::CreateGradientColorTable(Number,Stops,Red,Green,Blue,NColors);
 
 TColor::CreateGradientColorTable(__NRGBs, __stops, __red, __green, __blue, __NCont);
 gStyle->SetNumberContours(__NCont);
 

 
 
 //---- TBTree
 gSystem->Load("build/lib/libTB.so");  
 
 
 
}
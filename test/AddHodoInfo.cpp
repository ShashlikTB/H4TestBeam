#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "TTreeFormula.h"

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

//---- boost
// #include "boost/program_options.hpp"
// #include "boost/program_options/options_description.hpp"

// namespace po = boost::program_options;

#include <sstream> 

int main(int argc, char**argv){
 
 std::string input_file_shashlik;
 std::string input_file_beam;
 std::string output_file;
 int maxEvents = -1;
 
 //---- configuration
 
 int c;
 while ((c = getopt (argc, argv, "s:b:o:m:")) != -1)
  switch (c)
  {
   case 's': //---- shashlik
    input_file_shashlik = string(optarg);
    break;
   case 'b': //---- beam
    input_file_beam = string(optarg);
    break;
   case 'o':
    output_file =  string(optarg);
    break;
   case 'm':
    maxEvents =  atoi(optarg);
    break;
    
   case '?':
    if (optopt == 'b'|| optopt == 's'|| optopt == 'o'|| optopt == 'm')
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
  
  std::vector<std::string> input_files_beam_vector;
  std::stringstream ss(input_file_beam);
  
  std::string token_string;
  while(std::getline(ss, token_string, ',')) {
   std::cout << token_string << '\n';
   input_files_beam_vector.push_back(token_string);
  }
  
  std::vector<std::string> input_files_shashlik_vector;
  std::stringstream ss2(input_file_shashlik);
  
  while(std::getline(ss2, token_string, ',')) {
   std::cout << token_string << '\n';
   input_files_shashlik_vector.push_back(token_string);
  }
  
  
  std::cout << " input files:" << std::endl;
  for (int i=0; i<input_files_beam_vector.size(); i++) {
   std::cout << " beam: " << input_files_beam_vector.at(i) << std::endl;
  }
  for (int i=0; i<input_files_shashlik_vector.size(); i++) {
   std::cout << " shashlik: " << input_files_shashlik_vector.at(i) << std::endl;
  }
  
  //---- configuration (end)
  
  
  //---- read file
  
  TChain* H4tree_beam = new TChain("H4tree");
  for (unsigned int i=0; i<input_files_beam_vector.size(); i++) {
   H4tree_beam->Add(input_files_beam_vector.at(i).c_str());
  }
  
  TChain* H4tree_shashlik = new TChain("t1041");
  for (unsigned int i=0; i<input_files_shashlik_vector.size(); i++) {
   H4tree_shashlik->Add(input_files_shashlik_vector.at(i).c_str());
  }
  
   
  
  //---- variables 
  unsigned int runNumber ;
  unsigned int spillNumber ;
  unsigned int evtNumber ;
  unsigned int evtTimeDist ;
  unsigned int evtTimeStart ;
  
  unsigned int  nEvtTimes ;
  ULong64_t     evtTime [MAX_RO] ;
  unsigned int  evtTimeBoard [MAX_RO] ;
  
  unsigned int nAdcChannels ;
  unsigned int adcBoard[MAX_ADC_CHANNELS] ;
  unsigned int adcChannel[MAX_ADC_CHANNELS] ;
  unsigned int adcData[MAX_ADC_CHANNELS] ;
  
  unsigned int nDigiSamples ;
  unsigned int digiFrequency[MAX_DIGI_SAMPLES] ;
  unsigned int digiGroup[MAX_DIGI_SAMPLES] ;
  unsigned int digiChannel[MAX_DIGI_SAMPLES] ;
  unsigned int digiSampleIndex[MAX_DIGI_SAMPLES] ;
  unsigned int digiBoard[MAX_DIGI_SAMPLES] ;
  float digiSampleValue[MAX_DIGI_SAMPLES] ;
  
  unsigned int nTdcChannels ;
  unsigned int tdcBoard[MAX_TDC_CHANNELS] ;
  unsigned int tdcChannel[MAX_TDC_CHANNELS] ;
  unsigned int tdcData[MAX_TDC_CHANNELS] ;
  
  unsigned int nScalerWords ;
  unsigned int scalerWord[MAX_SCALER_WORDS] ;
  //  WORD scalerWord[MAX_SCALER_WORDS] ;
  unsigned int scalerBoard[MAX_SCALER_WORDS] ;
  
  unsigned int nPatterns ;
  unsigned int  pattern[MAX_PATTERNS] ;
  unsigned int  patternBoard[MAX_PATTERNS] ;
  unsigned int  patternChannel[MAX_PATTERNS] ;
  //  WORD pattern[MAX_PATTERNS] ;
  //  WORD patternBoard[MAX_PATTERNS] ;
  //  WORD patternChannel[MAX_PATTERNS] ;
  
  unsigned int nTriggerWords;
  unsigned int triggerWords[MAX_TRIG_WORDS] ;
  unsigned int triggerWordsBoard[MAX_TRIG_WORDS] ;
  
  
  H4tree_beam->SetBranchAddress("runNumber", &runNumber);
  H4tree_beam->SetBranchAddress("spillNumber", &spillNumber); 
  H4tree_beam->SetBranchAddress("evtNumber", &evtNumber);
  H4tree_beam->SetBranchAddress("evtTimeDist", &evtTimeDist);
  H4tree_beam->SetBranchAddress("evtTimeStart", &evtTimeStart);
  H4tree_beam->SetBranchAddress("nEvtTimes", &nEvtTimes);
  H4tree_beam->SetBranchAddress("evtTime", evtTime);
  H4tree_beam->SetBranchAddress("evtTimeBoard", evtTimeBoard);
  H4tree_beam->SetBranchAddress("nAdcChannels", &nAdcChannels);
  H4tree_beam->SetBranchAddress("adcBoard", adcBoard);
  H4tree_beam->SetBranchAddress("adcChannel", adcChannel);
  H4tree_beam->SetBranchAddress("adcData", adcData);
  H4tree_beam->SetBranchAddress("nTdcChannels", &nTdcChannels);
  H4tree_beam->SetBranchAddress("tdcBoard", tdcBoard);
  H4tree_beam->SetBranchAddress("tdcChannel", tdcChannel);
  H4tree_beam->SetBranchAddress("tdcData", tdcData);
  H4tree_beam->SetBranchAddress("nDigiSamples", &nDigiSamples);
  H4tree_beam->SetBranchAddress("digiFrequency", digiFrequency);
  H4tree_beam->SetBranchAddress("digiGroup", digiGroup);
  H4tree_beam->SetBranchAddress("digiChannel", digiChannel);
  H4tree_beam->SetBranchAddress("digiSampleIndex", digiSampleIndex);
  H4tree_beam->SetBranchAddress("digiSampleValue", digiSampleValue);
  H4tree_beam->SetBranchAddress("digiBoard", digiBoard);
  H4tree_beam->SetBranchAddress("nScalerWords", &nScalerWords);
  H4tree_beam->SetBranchAddress("scalerWord", scalerWord);
  H4tree_beam->SetBranchAddress("scalerBoard", scalerBoard);
  H4tree_beam->SetBranchAddress("nPatterns", &nPatterns);
  H4tree_beam->SetBranchAddress("pattern", pattern);
  H4tree_beam->SetBranchAddress("patternBoard", patternBoard);
  H4tree_beam->SetBranchAddress("patternChannel", patternChannel);
  H4tree_beam->SetBranchAddress("nTriggerWords", &nTriggerWords);
  H4tree_beam->SetBranchAddress("triggerWords", triggerWords);
  H4tree_beam->SetBranchAddress("triggerWordsBoard", triggerWordsBoard);
  
  
  //---- read file
  int nEntries = H4tree_shashlik->GetEntries(); 
  std::cout << " nEntries = " << nEntries << std::endl;
  if (maxEvents != -1) nEntries = maxEvents>nEntries ? nEntries : maxEvents ;
  std::cout << " new nEntries = " << nEntries << std::endl;
  
  
  TFile* output_file_root = new TFile (output_file.c_str(),"RECREATE");
  TTree* outtree = new TTree("t1041","t1041");
  TBSpill* tbspill = new TBSpill();
  TBEvent* tbevent = new TBEvent();
  
  TBranch *branch_event = H4tree_shashlik->GetBranch("tbevent");
  branch_event->SetAddress(&tbevent);
  TBranch *branch_spill = H4tree_shashlik->GetBranch("tbspill");
  branch_spill->SetAddress(&tbspill);
  
//   H4tree_shashlik->SetBranchAddress("tbspill",tbspill);
//   H4tree_shashlik->SetBranchAddress("tbevent",tbevent);
  
//   std::cout << " SetBranchAddress " << std::endl;

  TBSpill tbspill2;
  TBEvent tbevent2;
  outtree->Branch("tbspill", "TBSpill", &tbspill2, 64000, 0);
  outtree->Branch("tbevent", "TBEvent", &tbevent2, 64000, 0);
  
//   std::cout << " Branch " << std::endl;   
   
   
  int nEntries_beam = H4tree_beam->GetEntries();
  std::cout << " nEntries_beam " << nEntries_beam << std::endl;   
  
  
  ULong64_t start_event_beam_0;
  ULong64_t start_event_beam_1;
  ULong64_t start_event_beam_2;
  ULong64_t start_event_beam;
//   TTreeFormula* TTF = new TTreeFormula("ttf0","spillNumber==1 && evtNumber==1",H4tree_beam);
//   std::cout << " TTreeFormula " << std::endl;   
  
  for (int iBeam=0; iBeam<nEntries_beam; iBeam++) {
   H4tree_beam->GetEntry(iBeam);  
//    if (TTF->EvalInstance() == 1) {
   if (spillNumber==1 && evtNumber==1) {
    std::cout << " found: start_event_beam = " << start_event_beam << std::endl;
    start_event_beam = evtTime[0];
    start_event_beam_0 = evtTime[0];
    start_event_beam_1 = evtTime[1];
    start_event_beam_2 = evtTime[2];
    break;
   }
  }
  
  std::cout << " start_event_beam " << start_event_beam << std::endl;   

  
  H4tree_shashlik->GetEntry(0);
  ULong64_t start_event_shashlik;
  start_event_shashlik = tbevent->GetPadeChan(0).GetTimeStamp();
  
  std::cout << " start_event_shashlik " << start_event_shashlik << std::endl;   
  
  int Shashlik_spillNumber_old = -1;
  int Beam_spillNumber_old = -1;

  ULong64_t delta_time_shashlik;
  
  for (int i=0; i<nEntries; i++) {

   if ((i%4)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree_shashlik->GetEntry(i);

   tbevent2 = *tbevent;
   tbspill2 = *tbspill;
   
   int Shashlik_spillNumber = tbspill->GetSpillNumber();
   int Shashlik_eventNumber = -1;
   
//    std::cout << " Shashlik_spillNumber = " << Shashlik_spillNumber << std::endl;
//    if (Shashlik_spillNumber>1) break;
    
   if (Shashlik_spillNumber_old != Shashlik_spillNumber) {
    std::cout << " delta_time_shashlik old spill = " << delta_time_shashlik << std::endl;
   }
   
   if (tbevent->NPadeChan() != 0) {
    Shashlik_eventNumber = tbevent->GetPadeChan(0).GetEventNum();
    ULong64_t time_event_shashlik;
    time_event_shashlik = tbevent->GetPadeChan(0).GetTimeStamp();
    
    delta_time_shashlik = time_event_shashlik - start_event_shashlik;
   }

   if (Shashlik_spillNumber_old != Shashlik_spillNumber) {
    std::cout << " delta_time_shashlik new spill = " << delta_time_shashlik << std::endl;
    Shashlik_spillNumber_old = Shashlik_spillNumber;
   }
   
   
   
   //---- delta_time_shashlik expressed in 100 * \mu sec
//    delta_time_shashlik = delta_time_shashlik / 1000; //---- transform to ms
//    std::cout << "    delta_time_shashlik = " << delta_time_shashlik << std::endl;
//    delta_time_shashlik = delta_time_shashlik * 1000000; //---- transform to the same units as beam
//    delta_time_shashlik = delta_time_shashlik * 100; //---- transform to the same units as beam
//    delta_time_shashlik = delta_time_shashlik * 23.7; //---- transform to the same units as beam
   

   for (int iBeam=0; iBeam<nEntries_beam; iBeam++) {
    H4tree_beam->GetEntry(iBeam);
    if (Beam_spillNumber_old != spillNumber) {
    
     ULong64_t time_event_beam = evtTime[0];
     ULong64_t delta_time_beam = time_event_beam - start_event_beam;
     
     //     delta_time_beam = delta_time_beam/1000; //---- from ## to ms for beam data
     //     delta_time_beam = delta_time_beam/10; //---- from ## to ms for beam data
     //---- delta_time_beam expressed in \mu sec
     //     delta_time_beam = delta_time_beam/100; //---- from ms to 100*\mu s for beam data
     
     
     //---- delta_time_beam expressed in \mu sec
     //     std::cout << " iShashlik = " << i << std::endl;
     //     std::cout << " iBeam =     " << iBeam << std::endl;
     //     std::cout << "    delta_time_beam     = " << delta_time_beam   << " or " << evtTime[1]-start_event_beam_1 << " or " << evtTime[2] - start_event_beam_2  << std::endl;
     //     std::cout << "    delta_time_shashlik = " << delta_time_shashlik     << std::endl;
     //     std::cout << " abs(delta_time_shashlik - delta_time_beam) = " << abs(delta_time_shashlik - delta_time_beam) << std::endl;
     
     //     if (delta_time_shashlik == delta_time_beam) {
     
     //     if (abs(delta_time_shashlik - delta_time_beam)<(50 * 1000000)) { //---- Delta between electrons ~ 200 * 10^6 --> but 50 * 10^6 as time window
     if (abs(delta_time_shashlik - delta_time_beam) < (100 * 23.7)) { //---- Delta between electrons ~ 200 * 10^6 --> but 50 * 10^6 as time window
      //     if ((spillNumber == Shashlik_spillNumber) && (Shashlik_eventNumber == (evtNumber-1))) {
      std::cout << "  found  delta_time_beam     = " << delta_time_beam     << std::endl;
      std::cout << "  found  delta_time_shashlik = " << delta_time_shashlik << std::endl;
      tbevent2.SetWireChambersData(runNumber, spillNumber,evtNumber-1,tdcData,tdcBoard,tdcChannel,nTdcChannels);
      tbevent2.SetHodoScopeData(runNumber, spillNumber,evtNumber-1,pattern,patternBoard,patternChannel,nPatterns);
      break;
     }
    }
   }
   
   
   
   outtree->Fill();

  }
  outtree->Write();
  output_file_root->Close();
    
}




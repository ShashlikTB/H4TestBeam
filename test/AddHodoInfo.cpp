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
#include "include/TBRecHit.h"


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
  std::vector<TBRecHit> *rechits = new vector<TBRecHit>;
  
//   TBranch *branch_reco;
  bool haverechits = false;
  if(H4tree_shashlik->GetListOfBranches()->FindObject("tbrechits")) {
   std::cout << "found rechits" << std::endl;
//    branch_reco = H4tree_shashlik->GetBranch("tbrechits");
//    branch_reco->SetAddress(&rechits);  
   H4tree_shashlik->SetBranchAddress("tbrechits", &rechits);
   haverechits = true;
  }
  
  H4tree_shashlik->SetBranchAddress("tbevent", &tbevent);
  H4tree_shashlik->SetBranchAddress("tbspill", &tbspill);
  
//   TBranch *branch_event = H4tree_shashlik->GetBranch("tbevent");
//   branch_event->SetAddress(&tbevent);
//   TBranch *branch_spill = H4tree_shashlik->GetBranch("tbspill");
//   branch_spill->SetAddress(&tbspill);

  
  TBSpill tbspill2;
  TBEvent tbevent2;
  std::vector<TBRecHit> *rechits2 = new vector<TBRecHit>;
  
  outtree->Branch("tbspill", "TBSpill", &tbspill2, 64000, 0);
  outtree->Branch("tbevent", "TBEvent", &tbevent2, 64000, 0);
  if (haverechits) {
   outtree->Branch("tbrechits","std::vector<TBRecHit>",&rechits2);
  }
  
  
   
   
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
//    std::cout << "  spillNumber " << spillNumber << " evtNumber =  " << evtNumber << std::endl;
    
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
  int new_spill = 1;
  int new_spill_has_been_started = 1;
  
  int iBeam_Position_within_one_spill = 0;
  int iBeam_Position_of_the_spill = 0;
  
  int Shashlik_eventNumber_old = -1;
  int Beam_eventNumber_old = -1;
  
  ULong64_t delta_time_shashlik;
  
  for (int i=0; i<nEntries; i++) {
   
   if ((i%100)==0) {
    std::cout <<  " entry: " << i << "::" << nEntries << std::endl;
   }
   
   H4tree_shashlik->GetEntry(i);
   
   tbevent2 = *tbevent;
   tbspill2 = *tbspill;
   
   if (haverechits) {
    rechits2 = rechits;
   }
   
   int Shashlik_spillNumber = tbspill->GetSpillNumber();
   int Shashlik_eventNumber = -1;
   
   if (Shashlik_spillNumber_old != Shashlik_spillNumber) {
    new_spill = 1; //---- it's a new spill
    
    iBeam_Position_within_one_spill = 0;
    iBeam_Position_of_the_spill = 0;
    
    std::cout << " delta_time_shashlik old spill = " << delta_time_shashlik << " ## spillNumber = " << Shashlik_spillNumber << std::endl;
    
    if (tbevent->NPadeChan() != 0) {     
     ULong64_t time_event_shashlik;
     time_event_shashlik = tbevent->GetPadeChan(0).GetTimeStamp();
     
     delta_time_shashlik = (time_event_shashlik - start_event_shashlik);
     delta_time_shashlik = delta_time_shashlik / 10;  //---- convert between 100ns=0.1mus to 1mus;
    }
    
    std::cout << " delta_time_shashlik new spill = " << delta_time_shashlik << " ## spillNumber = " << Shashlik_spillNumber << std::endl;
    Shashlik_spillNumber_old = Shashlik_spillNumber;
   }
   
   
   
   //---- check if Shashlik skipped one or more events
   //---- if that is the case, skip the same number of event in the beam data
   Shashlik_eventNumber = tbevent->GetPadeChan(0).GetEventNum();
   int delta_event_in_Shashlik = Shashlik_eventNumber - Shashlik_eventNumber_old - 1;
   if (Shashlik_eventNumber_old != -1 && delta_event_in_Shashlik != 0) {
    iBeam_Position_within_one_spill += delta_event_in_Shashlik;
    std::cout << " delta_event_in_Shashlik = " << delta_event_in_Shashlik << " = " << Shashlik_eventNumber << " - " << Shashlik_eventNumber_old << " - 1  " << std::endl;
   }
   Shashlik_eventNumber_old = Shashlik_eventNumber;   
   //---- check if Shashlik skipped one event (end)
   
   
   //---- beam     time is in mus
   //---- shashlik time is in 100ns = 0.1mus
   //----    but before I converted everything into mus
   if (new_spill) {
    new_spill_has_been_started = 1;
    for (int iBeam=0; iBeam<nEntries_beam; iBeam++) {
     H4tree_beam->GetEntry(iBeam);
     //---- get to the correct spill number for the beam data
     //---- using itme information
     ULong64_t time_event_beam = evtTime[0];
     ULong64_t delta_time_beam = time_event_beam - start_event_beam;
     
     //---- 250000mus = 250 ms window match, to be large
     //---- it should get the first event in the spill from beam data
//      std::cout << " iBeam:: "<< iBeam << " :: abs(delta_time_shashlik - delta_time_beam) = " << delta_time_shashlik << " - " << delta_time_beam << " = " << delta_time_shashlik - delta_time_beam << std::endl;
     if ( (delta_time_shashlik >  delta_time_beam && (delta_time_shashlik - delta_time_beam) < 250000)  || (delta_time_shashlik <= delta_time_beam && (delta_time_beam - delta_time_shashlik) < 250000)) { 
      iBeam_Position_of_the_spill = iBeam; //---- save the position of the first event in the spill -> then in the loop start from this point!
      iBeam_Position_within_one_spill = 0; //---- found the correct position within one spill, set the counter to 0
      new_spill = 0;      
      Beam_spillNumber_old = spillNumber; //---- used to check that we are not in the next spill -> if it happens, don't fill the tree!
      std::cout << " >> got new spill in beam data ... " << std::endl;
      std::cout << " >> time = " << (delta_time_shashlik >  delta_time_beam) * (delta_time_shashlik - delta_time_beam)  +    (delta_time_shashlik <= delta_time_beam) * (delta_time_beam - delta_time_shashlik) << std::endl;
      std::cout << " >> spillNumber = " << spillNumber << std::endl;
      break;
     }
    }
   }
   else {
    new_spill_has_been_started = 0;
   }
   //---- get to the correct position within one spill
   H4tree_beam->GetEntry(iBeam_Position_of_the_spill + iBeam_Position_within_one_spill);
   

   //---- check if H4DAQ skipped one event
   //---- if that happened it means that PADE still got that event
   //---- but H4DAQ information is lost.
   //---- then we need to skip the matching for this PADE event
   //---- and iBeam_Position_within_one_spill is *not* increased
   //---- do this if we are not at the beginning of a new spill: new_spill_has_been_started != 0
   if (spillNumber == Beam_spillNumber_old) {//----                                                              if a shashlik event is dropped, then is expected a jump in beam data
    if ((new_spill_has_been_started == 0) && (Beam_eventNumber_old != -1) && (evtNumber != (Beam_eventNumber_old+1+delta_event_in_Shashlik))) {
     std::cout << " [attention] Beam data was skipped: " << evtNumber << " != ( " << Beam_eventNumber_old << " +1) in :: [spill shashlik = " << Shashlik_spillNumber << " ][shashlik Event = " << i << " :: " << Shashlik_eventNumber << "]" << std::endl;
     //---- skip (evtNumber-(Beam_eventNumber_old+1)) events, because those are the evetns skipped by beam data
     Beam_eventNumber_old = Beam_eventNumber_old+1;
    }
    else {   
     std::cout << " merged[spill:" << spillNumber << "]: Shashlik_eventNumber:Beam_evtNumber-1 = " << Shashlik_eventNumber << " : " << evtNumber-1 << std::endl;
     Beam_eventNumber_old = evtNumber;
     //---- if the beam moved to the next spill, fill the tree but not set hodoscope data
     //---- then the default values will appear, and we can filter a posteriori if needed
     tbevent2.SetWireChambersData(runNumber, spillNumber,evtNumber-1,tdcData,tdcBoard,tdcChannel,nTdcChannels);
     tbevent2.SetHodoScopeData(runNumber, spillNumber,evtNumber-1,pattern,patternBoard,patternChannel,nPatterns);
     //     std::cout << " iBeam_Position_of_the_spill = " << iBeam_Position_of_the_spill << std::endl;
     //     std::cout << " iBeam_Position_within_one_spill = " << iBeam_Position_within_one_spill << std::endl;
     iBeam_Position_within_one_spill++;
     //---- once we are in the correct position within one spill and we save information, we can exit from the loop
    }
   }
   else {
    std::cout << " Events not found in beam data for : [spill shashlik = " << Shashlik_spillNumber << " ][shashlik Event = " << i << " :: " << Shashlik_eventNumber << "]" << std::endl; 
   }
   
   
   outtree->Fill();
   
  }

  outtree->Write();
  output_file_root->Close();
    
}




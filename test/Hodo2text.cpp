#include <string>
#include "TTree.h"
#include "TFile.h"

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
#include "boost/program_options.hpp"
#include "boost/program_options/options_description.hpp"

namespace po = boost::program_options;


int main(int argc, char**argv){

 std::string input_file;
 std::string output_file;
 int maxEvents = -1;
 
 //---- configuration
 
 // Declare the supported options.
 po::options_description desc("Allowed options");
 desc.add_options()
    ("help", "produce help message")
    ("input_file",  po::value<std::string>(), "input file")
    ("output_file", po::value<std::string>(), "output file")
    ("maxEvents", po::value<int>(), "maxEvents")
 ;
 
 po::variables_map vm;
 po::store(po::parse_command_line(argc, argv, desc), vm);
 po::notify(vm);
 
 if (vm.count("help")) {
  std::cout << desc << "\n";
  return 1;
 }
 
 if (vm.count("input_file")) {
  input_file = vm["input_file"].as<std::string>();
 } else {
  std::cout << "defult?.\n";
 }
 
 if (vm.count("output_file")) {
  output_file = vm["output_file"].as<std::string>();
 } else {
  std::cout << "defult?.\n";
 }
 
 if (vm.count("maxEvents")) {
  maxEvents = vm["maxEvents"].as<int>();
 } else {
  std::cout << "maxEvents = -1 \n";
 }
 
 
 
 //---- configuration (end)
  
 
 //---- read file
 TFile* fileIn = new TFile (input_file.c_str(), "READ");
 TTree* H4tree = (TTree*) fileIn->Get("H4tree");
 
 
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
 
 
 
 
 H4tree->SetBranchAddress("evtNumber", &evtNumber);
 H4tree->SetBranchAddress("evtTimeDist", &evtTimeDist);
 H4tree->SetBranchAddress("evtTimeStart", &evtTimeStart);
 H4tree->SetBranchAddress("nEvtTimes", &nEvtTimes);
 H4tree->SetBranchAddress("evtTime", evtTime);
 H4tree->SetBranchAddress("evtTimeBoard", evtTimeBoard);
 H4tree->SetBranchAddress("nAdcChannels", &nAdcChannels);
 H4tree->SetBranchAddress("adcBoard", adcBoard);
 H4tree->SetBranchAddress("adcChannel", adcChannel);
 H4tree->SetBranchAddress("adcData", adcData);
 H4tree->SetBranchAddress("nTdcChannels", &nTdcChannels);
 H4tree->SetBranchAddress("tdcBoard", tdcBoard);
 H4tree->SetBranchAddress("tdcChannel", tdcChannel);
 H4tree->SetBranchAddress("tdcData", tdcData);
 H4tree->SetBranchAddress("nDigiSamples", &nDigiSamples);
 H4tree->SetBranchAddress("digiFrequency", digiFrequency);
 H4tree->SetBranchAddress("digiGroup", digiGroup);
 H4tree->SetBranchAddress("digiChannel", digiChannel);
 H4tree->SetBranchAddress("digiSampleIndex", digiSampleIndex);
 H4tree->SetBranchAddress("digiSampleValue", digiSampleValue);
 H4tree->SetBranchAddress("digiBoard", digiBoard);
 H4tree->SetBranchAddress("nScalerWords", &nScalerWords);
 H4tree->SetBranchAddress("scalerWord", scalerWord);
 H4tree->SetBranchAddress("scalerBoard", scalerBoard);
 H4tree->SetBranchAddress("nPatterns", &nPatterns);
 H4tree->SetBranchAddress("pattern", pattern);
 H4tree->SetBranchAddress("patternBoard", patternBoard);
 H4tree->SetBranchAddress("patternChannel", patternChannel);
 H4tree->SetBranchAddress("nTriggerWords", &nTriggerWords);
 H4tree->SetBranchAddress("triggerWords", triggerWords);
 H4tree->SetBranchAddress("triggerWordsBoard", triggerWordsBoard);
 
 
 //---- read file
 int nEntries = H4tree->GetEntries(); 
 std::cout << " nEntries = " << nEntries << std::endl;
 if (maxEvents != -1) nEntries = maxEvents>nEntries ? nEntries : maxEvents ;
 std::cout << "new nEntries = " << nEntries << std::endl;
 ofstream myfile;
 myfile.open (output_file);
 for (int i=0; i<nEntries; i++) {
  
  H4tree->GetEntry(i);
  
  myfile << "Event " << i << std::endl;  
  myfile << "  runNumber " << runNumber  << std::endl;
  myfile << "  spillNumber " << spillNumber << std::endl;
  myfile << "  evtNumber " << evtNumber << std::endl;
  myfile << "  evtTimeDist " << evtTimeDist << std::endl;
  myfile << "  evtTimeStart " << evtTimeStart << std::endl;
  
  myfile << "   nEvtTimes " << nEvtTimes  << std::endl;
   
  myfile << "  evtTime ";
  for (unsigned int i=0; i<nEvtTimes; i++ ) {
   myfile << evtTime[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  evtTimeBoard ";
  for (unsigned int i=0; i<nEvtTimes; i++ ) {
   myfile << evtTimeBoard[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  nAdcChannels " << nAdcChannels  << std::endl;
  
  myfile << "  adcBoard ";
  for (unsigned int i=0; i<nAdcChannels; i++ ) {
   myfile << adcBoard[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  adcChannel ";
  for (unsigned int i=0; i<nAdcChannels; i++ ) {
   myfile << adcChannel[i] << "    ";
  }  
  myfile << std::endl;
 
  myfile << "  adcData ";
  for (unsigned int i=0; i<nAdcChannels; i++ ) {
   myfile << adcData[i] << "    ";
  }  
  myfile << std::endl;
  
  
  myfile << "  nDigiSamples " << nDigiSamples  << std::endl;
  myfile << "  digiFrequency ";
  for (unsigned int i=0; i<nDigiSamples; i++ ) {
   myfile << digiFrequency[i] << "    ";
  }  
  myfile << std::endl;

  myfile << "  digiGroup ";
  for (unsigned int i=0; i<nDigiSamples; i++ ) {
   myfile << digiGroup[i] << "    ";
  }  
  myfile << std::endl;

  myfile << "  digiChannel ";
  for (unsigned int i=0; i<nDigiSamples; i++ ) {
   myfile << digiChannel[i] << "    ";
  }  
  myfile << std::endl;

  myfile << "  digiSampleIndex ";
  for (unsigned int i=0; i<nDigiSamples; i++ ) {
   myfile << digiSampleIndex[i] << "    ";
  }  
  myfile << std::endl;

  myfile << "  digiBoard ";
  for (unsigned int i=0; i<nDigiSamples; i++ ) {
   myfile << digiBoard[i] << "    ";
  }  
  myfile << std::endl;

  myfile << "  digiSampleValue ";
  for (unsigned int i=0; i<nDigiSamples; i++ ) {
   myfile << digiSampleValue[i] << "    ";
  }  
  myfile << std::endl;
  
  
  
  myfile << "  nTdcChannels " << nTdcChannels << std::endl;
  myfile << "  tdcBoard ";
  for (unsigned int i=0; i<nTdcChannels; i++ ) {
   myfile << tdcBoard[i] << "    ";
  }  
  myfile << std::endl;
  
  
  myfile << "  tdcChannel ";
  for (unsigned int i=0; i<nTdcChannels; i++ ) {
   myfile << tdcChannel[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  tdcData ";
  for (unsigned int i=0; i<nTdcChannels; i++ ) {
   myfile << tdcData[i] << "    ";
  }  
  myfile << std::endl;
  
  
  
  myfile << "  nScalerWords " << nScalerWords  << std::endl;
  myfile << "  scalerWord ";
  for (unsigned int i=0; i<nScalerWords; i++ ) {
   myfile << scalerWord[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  scalerBoard ";
  for (unsigned int i=0; i<nScalerWords; i++ ) {
   myfile << scalerBoard[i] << "    ";
  }  
  myfile << std::endl;
    
  myfile << "  nPatterns " << nPatterns  << std::endl;
  
  myfile << "  pattern ";
  for (unsigned int i=0; i<nPatterns; i++ ) {
   myfile << pattern[i] << "    ";
  }  
  myfile << std::endl;

  myfile << "  patternBoard ";
  for (unsigned int i=0; i<nPatterns; i++ ) {
   myfile << patternBoard[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  patternChannel ";
  for (unsigned int i=0; i<nPatterns; i++ ) {
   myfile << patternChannel[i] << "    ";
  }  
  myfile << std::endl;
  
  
  myfile << "  nTriggerWords " << nTriggerWords  << std::endl;
  myfile << "  triggerWords ";
  for (unsigned int i=0; i<nTriggerWords; i++ ) {
   myfile << triggerWords[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  triggerWordsBoard ";
  for (unsigned int i=0; i<nTriggerWords; i++ ) {
   myfile << triggerWordsBoard[i] << "    ";
  }  
  myfile << std::endl;
  
  myfile << "  End Event " << std::endl;
  
 }
 myfile.close(); 
 

 
}




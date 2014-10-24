#include <string>
#include "TTree.h"
#include "TFile.h"


//---- from TBtree Shashlik ----
#include "include/TBEvent.h"


#include "boost/program_options.hpp"
#include "boost/program_options/options_description.hpp"

namespace po = boost::program_options;


int main(int argc, char**argv){

 std::string input_file;
 std::string output_file;
 
 // Declare the supported options.
 po::options_description desc("Allowed options");
 desc.add_options()
    ("help", "produce help message")
    ("input_file",  po::value<std::string>(), "input file")
    ("output_file", po::value<std::string>(), "output file")
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
 
 
 TBSpill tbspill;
 
 std::cout << "ciao 1!" << std::endl;
 
 //---- read file
 TFile* fileIn = new TFile (input_file.c_str(), "READ");
 TTree* H4tree = (TTree*) fileIn->Get("H4tree");
 
//  H4tree->SetEntryList(0);
//  H4tree->Draw(">> myList","1","entrylist");
// //  H4tree->Draw(">> myList",globalCut.Data(),"entrylist");
//  TEntryList *myList = (TEntryList*) gDirectory->Get("myList");
//  H4tree->Draw("runNumber:spillNumber:evtNumber:evtTimeDist","","para goff");
//  int nEntries = myList->GetN();
 
 std::cout << "ciao!" << std::endl;
 int nEntries = H4tree->Draw("evtTime:evtNumber:evtTimeDist","","para goff");
//  int nEntries = H4tree->Draw("runNumber:spillNumber:evtNumber:evtTimeDist","","para goff");
 Double_t *v_evtTime = H4tree->GetV1();
 Double_t *v_evtNumber = H4tree->GetV2();
 
 std::cout << " nEntries = " << nEntries << std::endl;
 for (int i=0; i<nEntries; i++) {
//   std::cout << " v_evtTime[" << i << "]   = " << v_evtTime[i] << std::endl;
  std::cout << " v_evtNumber[" << i << "] = " << v_evtNumber[i] << std::endl;
 }
 
}




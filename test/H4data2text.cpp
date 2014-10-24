#include <string>
#include "TTree.h"
#include "TFile.h"

#include "boost/program_options.hpp"
#include "boost/program_options/options_description.hpp"

namespace po = boost::program_options;


int main(int argc, char**argv){

 std::string input_file;
 std::string output_file;
 
 // Declare the supported options.
 po::options_description desc("Allowed options");
//  desc.add_options()
//     ("help", "produce help message")
//     ("input_file",  po::value<std::string>(), "input file")
//     ("output_file", po::value<std::string>(), "output file")
//  ;
//  
//  po::variables_map vm;
//  po::store(po::parse_command_line(argc, argv, desc), vm);
//  po::notify(vm);
//  
//  if (vm.count("help")) {
//   std::cout << desc << "\n";
//   return 1;
//  }
//  
//  if (vm.count("input_file")) {
//   input_file = vm["input_file"].as<std::string>();
//  } else {
//   std::cout << "defult?.\n";
//  }
//  
//  if (vm.count("output_file")) {
//   output_file = vm["output_file"].as<std::string>();
//  } else {
//   std::cout << "defult?.\n";
//  }
 
 
 
 
 //---- read file
//  TFile* fileIn = new TFile (input_file.c_str(), "READ");
//  TTree* H4tree = (TTree*) fileIn->Get("H4tree");
 
 
 
 
 
 
}




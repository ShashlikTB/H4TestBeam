#ifndef CONVERTER_H
#define CONVERTER_H

#include <TFile.h>
#include <TTree.h>

#define DEBUG_CONVERTER 0
#define DEBUG_VERBOSE_CONVERTER 0


class converter{
 
public:
 
 converter();
 //  converter(std::ifstream *in, TFile *out, TTree * outTree);
 ~converter();
 
 //  // add a board if not existing
 //  int  AddBoard (boardHeader bH) ; 
 //  int  Unpack(int events);
 //  int UnpackEvents( WORD nevents, spillHeader *this_spill );
 //  static WORD GetBoardTypeId (WORD);
 //  static WORD GetBoardId (WORD);
 //  static WORD GetBoardCrateId (WORD);
 //  int UnpackBoards( WORD nboards );
 //  void CreateTree();
 //  void inline FillTree(){outTree_->Fill();};
 
private:
 
 //  map<WORD, Board *> boards_;
 //  
 //  std::ifstream *rawFile;
 //  Event * event_ ;
 //  TFile * outFile_;
 //  TTree * outTree_;
 //  UInt_t  boardId_;//probably useless
 //  UInt_t  boardType_;
 //  UInt_t  crateId_;
 //  //  Int_t  nBoardTypes_;
 
};

#endif



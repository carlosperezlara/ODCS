#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include <TApplication.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTab.h>
#include <TGTextEdit.h>
#include <TGNumberEntry.h>
#include <TGIcon.h>

#include <MappingTableCollection.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH2Poly.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TImage.h>

const Int_t kNumberOfBoards=9;
const Int_t kTotalNumberOfChannels=512;
const Int_t kNumberOfChannels=26;
const Int_t kNumberOfSamples=16;
const Int_t kMergeRefresh=1000; // noevents
const Int_t kNewEventRefresh=100; // noevents


//const Int_t kNumberOfChannels=26;
//const Int_t kNumberOfSamples=32;
//const Int_t kMergeRefresh=1000; // noevents
//const Int_t kNewEventRefresh=10; // noevents

class DataMonitor {
 private:
  void CreateScans(TGCompositeFrame *mf);
  void CreateDisplayConfiguration(TGCompositeFrame *mf);
  void CreateClusters(TGCompositeFrame *mf);
  void CreateChannels(TGCompositeFrame *mf);
  void CreateSignals(TGCompositeFrame *mf);
  void CreateHeights(TGCompositeFrame *mf);
  void CreateWidths(TGCompositeFrame *mf);
  void CreateHitSummary(TGCompositeFrame *mf);
  void CreateTimeSummary(TGCompositeFrame *mf);
  void CreateHistory(TGCompositeFrame *mf);
  const MappingPlane* GetMappingPlane(Int_t bd, TString sp, Int_t pl=0);
  void ReadPosition();
  void ReadConfig();
  void ConfigureChannels();
  void StampRun(TGCompositeFrame *mf);
  void ModelPads(Int_t bd);
  TString GetLocalCell(Int_t bd);
  void StyleH1(TH1D*);
  void GetXYFromCell(TString, Double_t&,Double_t&);
  TString GetCellName(Double_t,Double_t);
  void ReCreateHistograms();
  TApplication *fApp;
  TH1D *fChannel[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TProfile *fSignal[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TH1D *fHeight[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TH1D *fWidth[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TH1D *fClusters_Num[kNumberOfBoards];
  TH1D *fClusters_Wid[kNumberOfBoards];
  TH1D *fClusters_Amp[kNumberOfBoards];
  TH1D *fClusters_xCE[kNumberOfBoards];
  TProfile *fHitSummary[kNumberOfBoards];
  TProfile *fTimeSummary[kNumberOfBoards];
  TH1D *fTimeSummaryTemp;
  TH2Poly *fDiagrams[kNumberOfBoards];
  TCanvas *fCanvasMap;
  TCanvas *fCanvasMapS;
  TCanvas *fCanvasMapH;
  TCanvas *fCanvasMapW;
  TCanvas *fCanvasMapHS;
  TCanvas *fCanvasMapTS;
  TCanvas *fCanvasMapCF;
  TCanvas *fCanvasMapSC;
  TCanvas *fCanvasMapHI;
  TCanvas *fCanvasMapCL;
  TGraph *fHistory[kNumberOfBoards];
  TGMainFrame *fWindowHitSummary;
  TGMainFrame *fWindowTimeSummary;
  TGMainFrame *fWindowDetails;
  TGTextEdit *fConfigurationFile;
  MappingTableCollection *fMapCollection;
  TH2D *fScan[kNumberOfBoards];
  
  TTimer *fRefresh;
  Double_t fPedestals[kNumberOfBoards][kNumberOfChannels];
  Int_t fThisEvent;
  Int_t fNoEventsSampled;
  Double_t fEventsReaded;
  Double_t fEventsCataloged;
  TString fClosestCell;
  Double_t fPosX;
  Double_t fPosY;
  TString fBoardCode[kNumberOfBoards];
  TString fBoardTech[kNumberOfBoards];
  Int_t fBoardAngle[kNumberOfBoards];
  TString fBoardCELL[kNumberOfBoards];
  Double_t fPitchX[kNumberOfBoards][kNumberOfChannels];
  Double_t fPeriodY[kNumberOfBoards][kNumberOfChannels];
  Double_t fStretch[kNumberOfBoards][kNumberOfChannels];
  Int_t fDREAMChannels[kNumberOfBoards];
  Int_t fDREAMChannel[kNumberOfBoards][kNumberOfChannels];
  Bool_t fNotInstalled[kNumberOfBoards];
  Double_t fIntWindow[kNumberOfBoards];
  Bool_t fReady;
  
 public:
  DataMonitor(TApplication *app, UInt_t w, UInt_t h);
  virtual ~DataMonitor();
  void RefreshAll();
  TH1D* GetChannel(int b, int s) {return fChannel[b][s];}
  TH2D* GetScan(int b) {return fScan[b];}
  void NewRun(Int_t run);
  void NewEvent(Int_t evr);
  void Merge();
  void SetDREAMChannel(Int_t bd,Int_t ch, Int_t idx) {fDREAMChannel[bd][ch] = idx;}
  Int_t GetDREAMChannels(Int_t bd) {return fDREAMChannels[bd];}
  Int_t GetDREAMChannel(Int_t bd,Int_t ch) {return fDREAMChannel[bd][ch];}
  Int_t IsBoardNotInstalled(Int_t bd) {return fNotInstalled[bd];}

  
  TGLabel *fThisRun;
  TGLabel *fEventsSampled;
  TGLabel *fSamplingFraction;
  TGTab *fTabContainer;
  TGTab *fTabContainer2;
  
  ClassDef(DataMonitor, 0)
};

#endif

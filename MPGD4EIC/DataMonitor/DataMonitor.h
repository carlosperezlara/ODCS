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

#include <TH1D.h>
#include <TH2Poly.h>
#include <TProfile.h>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TImage.h>

const Int_t kNumberOfChannels=26;
const Int_t kNumberOfBoards=8;
const Int_t kNumberOfSamples=256;
const Int_t kMergeRefresh=1000; // noevents
const Int_t kNewEventRefresh=100; // noevents

class DataMonitor {
 private:
  void CreateDisplayConfiguration(TGCompositeFrame *mf);
  void CreateChannels(TGCompositeFrame *mf);
  void CreateSignals(TGCompositeFrame *mf);
  void CreateHeights(TGCompositeFrame *mf);
  void CreateWidths(TGCompositeFrame *mf);
  void CreateHitSummary(TGCompositeFrame *mf);
  void CreateTimeSummary(TGCompositeFrame *mf);
  void ReadConfig(TString);
  void StampRun(TGCompositeFrame *mf);
  void ModelPads(Int_t bd);
  TApplication *fApp;
  TH1D *fChannel[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TProfile *fSignal[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TH1D *fHeight[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TH1D *fWidth[kNumberOfBoards][kNumberOfChannels]; // display kNumberOfChannels strips
  TH1D *fHitSummary[kNumberOfBoards];
  TH1D *fTimeSummary[kNumberOfBoards];
  TH1D *fTimeSummaryTemp;
  TH2Poly *fDiagrams[kNumberOfBoards];
  TCanvas *fCanvasMap;
  TCanvas *fCanvasMapS;
  TCanvas *fCanvasMapH;
  TCanvas *fCanvasMapW;
  TCanvas *fCanvasMapHS;
  TCanvas *fCanvasMapTS;
  TCanvas *fCanvasMapCF;
  TGMainFrame *fWindowHitSummary;
  TGMainFrame *fWindowTimeSummary;
  TGMainFrame *fWindowDetails;
  TGTextEdit *fConfigurationFile;
  
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
  Double_t fPitchX[kNumberOfChannels];
  Double_t fPeriodY[kNumberOfChannels];
  Double_t fStretch[kNumberOfChannels];

 public:
  DataMonitor(TApplication *app, UInt_t w, UInt_t h);
  virtual ~DataMonitor();
  void RefreshAll();
  TH1D *GetChannel(int b, int s) {return fChannel[b][s];}
  void NewRun(Int_t run);
  void NewEvent();
  void Merge();
  TGLabel *fThisRun;
  TGLabel *fEventsSampled;
  TGLabel *fSamplingFraction;
  TGTab *fTabContainer;
  TGTab *fTabContainer2;
  
  ClassDef(DataMonitor, 0)
};

#endif

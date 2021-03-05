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
#include <TF1.h>
#include <TH2D.h>
#include <TH2Poly.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TImage.h>
#include "Reader.h"
#include "WaveForm.h"

const Int_t kMergeRefresh=1000; // noevents
const Int_t kNewEventRefresh=100; // noevents

class DataMonitor {
 public:
  DataMonitor(TApplication *app, Int_t run, UInt_t w, UInt_t h);
  virtual ~DataMonitor();
  
 private:
  void LinkData(Int_t);
  void LinkV1742(Int_t);
  void LinkOSC(Int_t);
  void LinkSimulation();
  void StyleH1(TH1*,Int_t col=kBlue-3,Bool_t Fill=kTRUE);
  void CreateRawWaves(TGCompositeFrame *mf);
  void CreateRawWavesBAR(TGCompositeFrame *mf,Int_t ibar);  
  void CreateRawWavesHDR(TGCompositeFrame *mf,Int_t ibar,Int_t ihdr);  
  void CreateBarCorrelations(TGCompositeFrame *mf,Int_t ibar);  
  void ReCreateHistograms();
  void RefreshAll();
  void EventLoop();
  void OneSimEvent();
  TF1* CreateDeltaFit(TString name, TH1D *hist);

  TApplication *fApp;
  Int_t fRun;
  TGMainFrame *fWindowSummary;

  TH2D *fSiPM_WaveRaw_2D[16]; //2*4*2
  TProfile *fSiPM_WaveRaw_WaProf[16]; //2*4*2
  TH1D *fSiPM_WaveRaw_PedMea[16]; //2*4*2
  TH1D *fSiPM_WaveRaw_PedSig[16]; //2*4*2

  TGraph *fAmperimeter[4][2];
  TGraph *fTermistor[4][2];

  TH2D *fBar_CorrEner[4];
  TH2D *fBar_CorrTime[4];
  TH1D *fBar_ITimeRes[4];
  TH1D *fBar_ITimeRes10[4];
  TH1D *fBar_ITimeRes30[4];
  TH1D *fBar_ITimeRes50[4];
  TF1  *fBar_ITimeResFit[4][4];
  
  TCanvas *fCanvasRawWaves[4][2];
  TCanvas *fCanvasBarCorrelations[4];
  TCanvas *fCanvasSensors[4][2];

  Reader *fDRS;
  Reader *fOSC[4];
  WaveForm *fDRSChannel[16];

  Double_t fCalib_HLR[4][2];
  Double_t fBinWidth; // bin width in ns
  
  /*
  TGLabel *fThisRun;
  TGLabel *fEventsSampled;
  TGLabel *fSamplingFraction;
  
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
  TGMainFrame *fWindowTimeSummary;
  TGMainFrame *fWindowDetails;
  TGTextEdit *fConfigurationFile;
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
  */  
  
  ClassDef(DataMonitor, 0)
};

#endif

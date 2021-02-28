#ifndef DAQCONTROL_H
#define DAQCONTROL_H

#include <vector>

#include <TApplication.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEdit.h>
#include <TGNumberEntry.h>
#include <TGIcon.h>

#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TImage.h>


class DAQControl : public TGMainFrame {
 private:
  TApplication *fApp;
  TString sPath;
  Int_t fRunNumber;
  
  TGTextButton *fLock;
  TGTextButton *fUnlock;
  TGTextButton *fStartRun;
  TGTextButton *fStopRun;
  TGLabel *fFileName;
  TGLabel *fFileSize;
  TGLabel *fFileTS;
  TGLabel *fLabRunNumber;
  TGTextEdit *fConfigFile;
  TGLabel *fTGTermistor[4][2];
  TGLabel *fTGCurrent[4][2];
  TGLabel *fTGVoltage[4][2];
  
  Int_t fBarColor[4];
  TGIcon *fIcon[4];
  Double_t fTemperature;
  Double_t fAngle[4];
  Double_t fSiPM_Vbr[4][2];
  Double_t fSiPM_OV[4][2];
  TString  fTermistor[4][2];
  TString  fCurrent[4][2];
  
  TGNumberEntry *fTGAngle[4];
  TGNumberEntry *fTGSiPM_Vbr[4][2];
  TGNumberEntry *fTGSiPM_OV[4][2];
  
  TGraph *fBarDrawing[4];
  TCanvas *fCanvasMap;
  TCanvas *fCanvasImg;
  TGraph *fPointerObj;
  TGraph *fPointerMust;
  TTimer *fCallReadPositions;
  TTimer *fCallBusy;
  TTimer *fCaptain;

  Pixel_t fPixelRed;
  Pixel_t fPixelWhite;
  Pixel_t fPixelBlue;
  Pixel_t fPixelBlack;
  Pixel_t fPixelGreen;
  Pixel_t fPixelDefaultBgr;

  int fYouMayFireWhenReady;

  void CreateTab1(TGCompositeFrame*);
  void CreateEyes(TGCompositeFrame*);
  void CreateAngle(TGCompositeFrame*);
  void CreateVoltageControler(TGCompositeFrame*);
  void CreateVoltageControl(TGCompositeFrame*,Int_t, Bool_t up=kTRUE);
  void CreatePlot(TGCompositeFrame*);
  void CreateTermistor(TGCompositeFrame*);
  void CreateRunControl(TGCompositeFrame*);
  void CreateConfigFile(TGCompositeFrame*);
  
 public:
  DAQControl(TApplication *app, UInt_t w, UInt_t h);
  virtual ~DAQControl();
  void SetV(int);
  void SetAngle(int);
  void Lock();
  void Unlock();
  void StartRun();
  void StopRun();
  void ReadTermistors();
  void ShowConfigFile();
  void LoadConfigFile();
  void SaveConfigFile();

  ClassDef(DAQControl, 0)
};

#endif

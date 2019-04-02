#ifndef XYTABLE_H
#define XYTABLE_H

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

#include "Velmex.h"
#include "Mitutoyo.h"

class XYTable : public TGMainFrame {
 private:
  TApplication *fApp;
  TString sPath;
  Double_t fPreLoaded[10][19][2];

  TGTextButton     *fCell[10][19];
  TGTextButton     *fMove;
  TGTextButton     *fMoveS;
  TGTextButton     *fReset;
  TGTextButton     *fCancel;
  TGTextButton     *fClose;
  TGLabel *fGLXnow;
  TGLabel *fGLYnow;
  TGLabel *fGLXsta;
  TGLabel *fGLYsta;
  TGTextEdit *fGTXlog;
  TGTextEdit *fGTYlog;
  TGNumberEntry *fGTXobj;
  TGNumberEntry *fGTYobj;
  TGIcon *fIcon;
  TGLabel *fMotorX;
  TGLabel *fMotorY;
  TGLabel *fMotorXLU;
  TGLabel *fMotorYLU;

  Bool_t fMovingOperation;
  Bool_t fLock;
  int fXnow;
  int fYnow;
  int fXmust;
  int fYmust;
  int fXobj;
  int fYobj;
  Double_t fDXnow;
  Double_t fDYnow;
  Double_t fDXobj;
  Double_t fDYobj;
  Double_t fDXmust;
  Double_t fDYmust;
  Double_t fSPMX;
  Double_t fSPMY;
  Double_t fAngY;
  std::vector<TString> fCells;
  
  TCanvas *fCanvasMap;
  TCanvas *fCanvasImg;
  TImage *fImg;
  TGraph *fPointer;
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
  Velmex *fMotor;
  Mitutoyo *fEncoder;
  int fYouMayFireWhenReady;

  void CreateTab1(TGCompositeFrame*);
  void CreateTab2(TGCompositeFrame*);
  void CreateControl(TGCompositeFrame*);
  void CreateManualControl(TGCompositeFrame*);
  void CreateScriptControl(TGCompositeFrame*);
  void CreatePreLoadedTable(TGCompositeFrame*);
  void CreateStatusTextXY(TGCompositeFrame*);
  void CreateControlButtons(TGCompositeFrame*,TGCompositeFrame*);
  void CreateEyes(TGCompositeFrame*);
  void CreatePlot(TGCompositeFrame*);
  void CreateMotorInspection(TGCompositeFrame*);
  TString WhereAmI();
  void Look(Bool_t val=true) {Log( val?"Locked":"Unlocked"); fLock=val;}
  void Log(TString val);
  
 public:
  XYTable(TApplication *app, UInt_t w, UInt_t h);
  virtual ~XYTable();
  void ChangeCoordsFromCell(const char* rc);
  void SetOutOfWay();
  void PrepareMove();
  void SetObj();
  void SetXY();
  void UpdateXYState();
  void UpdatePointer();
  void MoveXY();
  void MoveStart();
  void ResetXY();
  void CancelXY();
  void LoadLogX();
  void LoadLogY();
  void LoadLogs();
  void ReadPositions();
  void ReadBusy();

  ClassDef(XYTable, 0)
};

#endif

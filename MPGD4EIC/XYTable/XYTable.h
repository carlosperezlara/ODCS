#ifndef XYTABLE_H
#define XYTABLE_H

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

#include "Velmex.h"
#include "Mitutoyo.h"

class XYTable : public TGMainFrame {
 private:
  TApplication *fApp;
  TString sPath;
  Int_t fPreLoaded[10][19][2];

  TGTextButton     *fCell[10][19];
  TGTextButton     *fMove;
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

  Bool_t fLock;
  int fXnow;
  int fYnow;
  int fXmust;
  int fYmust;
  int fXobj;
  int fYobj;
  Double_t fDXnow;
  Double_t fDYnow;
  Double_t fDXmust;
  Double_t fDYmust;

  TCanvas *fCanvasMap;
  TGraph *fPointer;
  TGraph *fPointerObj;
  TGraph *fPointerMust;
  TTimer *fCallReadPositions;
  Pixel_t fPixelRed;
  Pixel_t fPixelBlue;
  Pixel_t fPixelBlack;
  Pixel_t fPixelGreen;
  Pixel_t fPixelDefaultBgr;
  Velmex *fMotor;
  Mitutoyo *fEncoder;

  void CreateTab1(TGCompositeFrame*);
  void CreateTab2(TGCompositeFrame*);
  void CreateControl(TGCompositeFrame*);
  void CreateManualControl(TGCompositeFrame*);
  void CreateScriptControl(TGCompositeFrame*);
  void CreatePreLoadedTable(TGCompositeFrame*);
  void CreateControlTextX(TGCompositeFrame*);
  void CreateControlTextY(TGCompositeFrame*);
  void CreateControlButtons(TGCompositeFrame*,TGCompositeFrame*);
  void CreateEyes(TGCompositeFrame*);
  void CreatePlot(TGCompositeFrame*);
  void CreateMotorInspection(TGCompositeFrame*);
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
  void ReadRawPositions();
  void MoveXY();
  void ResetXY();
  void CancelXY();
  void LoadLogX();
  void LoadLogY();
  void LoadLogs();
  void ReadPositions();  

  ClassDef(XYTable, 0)
};

#endif

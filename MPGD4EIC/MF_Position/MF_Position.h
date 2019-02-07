#ifndef MF_POSITION
#define MF_POSITION

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

#include "MC_Velmex.h"
#include "LE_Mitutoyo.h"

class MF_Position : public TGMainFrame {
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

  int fXnow;
  int fYnow;
  int fXmust;
  int fYmust;
  int fXobj;
  int fYobj;

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
  MC_Velmex *fMotor;
  LE_Mitutoyo *fEncoder;

  void CreateTab1(TGCompositeFrame*);
  void CreateTab2(TGCompositeFrame*);

  void CreateCellArray(TGCompositeFrame*);
  void CreateControlTextX(TGCompositeFrame*);
  void CreateControlTextY(TGCompositeFrame*);
  void CreateControlButtons(TGCompositeFrame*,TGCompositeFrame*);
  void CreateEyes(TGCompositeFrame*);
  void CreatePlot(TGCompositeFrame*);
  void CreateMotorInspection(TGCompositeFrame*);

 public:
  MF_Position(TApplication *app, UInt_t w, UInt_t h);
  virtual ~MF_Position();
  void ChangeCoordsFromCell(const char* rc);
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

  ClassDef(MF_Position, 0)
};

#endif

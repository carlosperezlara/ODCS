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
  Int_t fPreLoaded[10][10][2];
  TGTextButton     *fCell[10][10];
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

  void CreateCellArray(TGCompositeFrame*);
  void CreateControlTextX(TGCompositeFrame*);
  void CreateControlTextY(TGCompositeFrame*);
  void CreateControlButtons(TGCompositeFrame*,TGCompositeFrame*);
  void CreateEyes(TGCompositeFrame*);

 public:
  MF_Position(TApplication *app, UInt_t w, UInt_t h);
  virtual ~MF_Position();
  void ChangeCoordsFromCell(const char* rc);
  void PrepareMove();
  void SetObj();
  void SetXY();
  void UpdateXYState();
  void UpdatePointer();
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

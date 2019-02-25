#ifndef VELMEX_H
#define VELMEX_H
#include "TString.h"
#include "StandardDeviceConnection.h"

class Velmex : public StandardDeviceConnection {
 protected:
  void Execute(TString);
  void ExecuteMoveRelative(Int_t midx, Double_t nsteps);
  void ExecuteMoveRelative(Int_t midx, Double_t nsteps, Int_t midx2, Double_t nsteps2);
  
  Bool_t   fIsConnected;
  Double_t fStepsPerUnit[4];
  Int_t    fStepsPerSecond[4];

 public:
  Velmex(TString dev="/dev/cu.usbserial");
  virtual ~Velmex();
  void Connect();
  void Disconnect();
  void Abort();
  void MoveRelative(Int_t midx, Double_t units);
  void MoveRelative(Int_t midx, Double_t units,Int_t midx2, Double_t units2);
  void SetStepsPerMilimiter(Int_t midx, Double_t val) { fStepsPerUnit[midx] = val; }
  void SetStepsPerSecond(Int_t midx, Int_t val) { fStepsPerSecond[midx] = val; }
  Bool_t IsReady() {return kTRUE;}
  TString GetCurrentPosition(Int_t midx);
  TString GetStatus();
  
  ClassDef(Velmex, 0)
};

#endif

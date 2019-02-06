#ifndef MC_VELMEX
#define MC_VELMEX
#include "TString.h"
#include "StandardDeviceConnection.h"

class MC_Velmex {
 protected:
  void Execute(TString);
  void ExecuteMoveRelative(Int_t midx, Int_t nsteps);
  void ExecuteMoveRelative(Int_t midx, Int_t nsteps, Int_t midx2, Int_t nsteps2);
  
  StandardDeviceConnection *fDevice;
  Bool_t  fIsConnected;
  Int_t   fStepsPerUnit[4];

 public:
  MC_Velmex(TString dev="/dev/cu.usbserial");
  virtual ~MC_Velmex();
  void Connect();
  void Disconnect();
  void Abort();
  void MoveRelative(Int_t midx, Int_t units);
  void MoveRelative(Int_t midx, Int_t units,Int_t midx2, Int_t units2);
  void SetStepsPerUnit(Int_t midx, Int_t val) { fStepsPerUnit[midx] = val; }
  Bool_t IsReady() {return kTRUE;}
  
  ClassDef(MC_Velmex, 0)
};

#endif

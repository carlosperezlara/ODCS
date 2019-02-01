#ifndef MC_VELMEX
#define MC_VELMEX
#include "TString.h"
#include "StandardDeviceConnection.h"

class MC_Velmex {
 protected:
  void Execute(TString);
  void ExecuteMoveRelative(Int_t midx, Int_t nsteps);
  
  StandardDeviceConnection *fDevice;
  Bool_t  fIsConnected;
  Int_t   fStepsPerUnit;

 public:
  MC_Velmex(TString dev="/dev/cu.usbserial");
  virtual ~MC_Velmex();
  void Connect();
  void Disconnect();
  void Abort();
  void MoveRelative(Int_t midx, Int_t units);
  void SetStepsPerUnit(Int_t val) { fStepsPerUnit = val; }

  ClassDef(MC_Velmex, 0)
};

#endif

#ifndef LE_MITUTOYO
#define LE_MITUTOYO
#include "TString.h"
#include "StandardDeviceConnection.h"

class LE_Mitutoyo {
 protected:
  void Execute(TString);
  void ExecuteMoveRelative(Int_t midx, Int_t nsteps);
  
  StandardDeviceConnection *fDevice;
  Bool_t  fIsConnected;
  Int_t   fStepsPerUnit;

 public:
  LE_Mitutoyo(TString dev="/dev/cu.usbserial");
  virtual ~LE_Mitutoyo();
  void Connect();
  void Disconnect();
  void Abort();
  void MoveRelative(Int_t midx, Int_t units);
  void SetStepsPerUnit(Int_t val) { fStepsPerUnit = val; }
  Bool_t IsReady() {return kTRUE;}
  
  ClassDef(LE_Mitutoyo, 0)
};

#endif

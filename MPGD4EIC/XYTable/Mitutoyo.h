#ifndef MITUTOYO_H
#define MITUTOYO_H
#include "TString.h"
#include "StandardDeviceConnection.h"

class Mitutoyo : public StandardDeviceConnection {
 protected:

 public:
  Mitutoyo(TString dev="/dev/cu.usbserial");
  virtual ~Mitutoyo();
  void ReadXY(Int_t &x, Int_t &y);
  
  ClassDef(Mitutoyo, 0)
};

#endif

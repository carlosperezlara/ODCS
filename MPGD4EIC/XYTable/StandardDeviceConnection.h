#ifndef STDDEVCON
#define STDDEVCON

#include <iostream>
#include <TString.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

class StandardDeviceConnection {
 protected:
  TString fDevice;
  Int_t   fFileDescriptor;
  Int_t   fMode;
  Int_t   fNULL;

 public:
  StandardDeviceConnection();
  StandardDeviceConnection(TString,Int_t);
  virtual ~StandardDeviceConnection();
  int Error() {return fNULL;}
  void SetDeviceAddress(TString val) {fDevice = val;}
  void SetDeviceAccessMode(Int_t val) {fMode = val;}
  void Init();
  void Send(TString);
  TString Receive();
  void Set7E1at4800();
  void Flush();
  
  ClassDef(StandardDeviceConnection, 0)
};
#endif


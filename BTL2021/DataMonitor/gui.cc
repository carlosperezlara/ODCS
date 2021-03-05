#include <iostream>
#include "DataMonitor.h"
#include <TApplication.h>
#include <TString.h>

DataMonitor *gDM;

//======================
//======================
//======================
int main(int nn, char** arg) {
  Int_t iRun = 0;
  if(nn>1) {
    TString sRun = arg[1];
    iRun = sRun.Atoi();
  }
  
  TApplication *app = new TApplication("gui",0,0);
  gDM = new DataMonitor(app, iRun, 1600, 850);
  
  app->Run();
  return 0;
}

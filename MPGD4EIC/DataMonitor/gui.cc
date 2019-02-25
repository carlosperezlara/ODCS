#include <TApplication.h>
#include <pmonitor/pmonitor.h>
#include "DataMonitor.h"

DataMonitor *dm;

//======================
int pinit() {
  return 0;
}
//======================
int process_event (Event * e) {
  Packet *p = e->getPacket(1003);
  if(p) {
    dm->GetChannel()->Fill ( p->iValue(0) );
    //_h2->Fill ( p->iValue(1) );
    delete p;
  }
  return 0;
}
//======================
//======================
//======================
int main() {
  TApplication *app = new TApplication("gui",0,0);
  dm = new DataMonitor(app, 850, 850);
  ptestopen();
  pstart();
  app->Run();
  return 0;
}

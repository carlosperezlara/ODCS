#include <TApplication.h>
#include "DAQControl.h"

//====================
//====================
//====================
//====================
int main() {
  TApplication *app = new TApplication("gui",0,0);
  new DAQControl(app, 1050, 850);
  app->Run();
  return 0;
}

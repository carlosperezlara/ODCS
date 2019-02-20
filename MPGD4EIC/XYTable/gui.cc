#include <TApplication.h>
#include "MF_Position.h"

//====================
//====================
//====================
//====================
int main() {
  TApplication *app = new TApplication("gui",0,0);
  new MF_Position(app, 850, 850);
  app->Run();
  return 0;
}

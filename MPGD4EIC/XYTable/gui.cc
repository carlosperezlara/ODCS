#include <TApplication.h>
#include "XYTable.h"

//====================
//====================
//====================
//====================
int main() {
  TApplication *app = new TApplication("gui",0,0);
  new XYTable(app, 850, 850);
  app->Run();
  return 0;
}

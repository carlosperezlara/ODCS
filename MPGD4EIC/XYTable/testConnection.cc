#include <iostream>
#include <TString.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "StandardDeviceConnection.h"

int main() {
  if(0) {
    StandardDeviceConnection test("/dev/stdout",O_WRONLY);
    test.Send("HOLA");
  }

  if(0) { // velmex
    StandardDeviceConnection test("/dev/ttyUSB0",O_RDONLY | O_NONBLOCK );
    TString str;
    bool exit = false;
    bool started = false;
    while(!exit) {
      str = test.Receive(1);
      if(str.Length()==0) {
	if(started) exit = true;
	continue;
      }
      started = true;
      std::cout << str.Data();
    }
    std::cout << std::endl;
  }

  if(1) { // mitutoyo
    //StandardDeviceConnection test("/dev/ttyUSB1",O_RDONLY | O_NONBLOCK);
    StandardDeviceConnection test("/dev/ttyUSB1",O_RDONLY | O_NOCTTY | O_NDELAY);
    test.Set7E1at4800();
    TString str;
    bool exit = false;
    bool started = false;
    TString all="0123456789";
    while(!exit) {
      str = test.Receive(1);
      if(str.Length()==0) {
	//if(started) exit = true;
	continue;
      }
      if(all.Contains(str[0]))
	started = true;
      std::cout << str.Data();
    }
    std::cout << std::endl;
  }

  return 0;
}


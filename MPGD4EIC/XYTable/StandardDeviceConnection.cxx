#include <iostream>
#include <TString.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "StandardDeviceConnection.h"

ClassImp(StandardDeviceConnection);

//======
StandardDeviceConnection::StandardDeviceConnection() {
  fDevice = "/dev/NULL";
  fFileDescriptor = -1;
  fMode = O_RDWR;
}
//======
StandardDeviceConnection::StandardDeviceConnection(TString dev, Int_t mode) {
  fDevice = dev;
  fFileDescriptor = -1;
  fMode = mode;
  Init();
}
//======
void StandardDeviceConnection::Init() {
  fFileDescriptor = open(fDevice.Data(), fMode);
  if(fFileDescriptor<0) {
    std::cout << "StandardDeviceConnection::Init() cannot open device!" << std::endl;
  }
}
//======
void StandardDeviceConnection::Send(TString word) {
  if(fFileDescriptor<0) {
    std::cout << "StandardDeviceConnection::Send() no valid file descriptor, forgot to call Init()?" << std::endl;
    return;
  }
  ssize_t len = write(fFileDescriptor, word, word.Length());
  if(len!=word.Length()) {
    std::cout << "StandardDeviceConnection::Send() was interrupted!" << std::endl;
  }
}
//======
TString StandardDeviceConnection::Receive(Int_t nbytes) {
  if(fFileDescriptor<0) {
    std::cout << "StandardDeviceConnection::Receive() no valid file descriptor, forgot to call Init()?" << std::endl;
    return "";
  }
  if(nbytes>=100) {
    return "";
  }
  char word[100];
  ssize_t len = read(fFileDescriptor, word, nbytes);
  Int_t lent = len;
  if(lent!=nbytes) {
    std::cout << "StandardDeviceConnection::Send() was interrupted!" << std::endl;
    return "";
  }
  TString mask = "+-0123456789";
  TString sword = "";
  Bool_t flag = false;
  int retreived = 0;
  for(int i=0; i!=nbytes; ++i) {
    if(!mask.Contains(word[i])) {
      flag = true;
      break;
    }
    sword += word[i];
  }
  if(flag) {
    //std::cout << "StandardDeviceConnection::Send() error while reading " << nbytes << " bytes:" << std::endl;
    //std::cout << "Readable word: " << sword.Data() << std::endl;
    //std::cout << "Full string: ";
    int max = nbytes;
    for(int i=0; i!=max; ++i) {
      if(word[i]==13) {
	max++;
	if(max>100) break;
	continue;
      }
      std::cout << Form("%d ",word[i]);
    }
    std::cout << std::endl;
    return "";
  }
  return sword;
}

/*
int main() {
  //StandardDeviceConnection test;
  //test.SetDeviceAddress("/dev/stdout");
  //test.SetDeviceAccessMode(O_WRONLY);
  //test.Init();

  StandardDeviceConnection test("/dev/stdout",O_WRONLY);
  test.Send("HOLA");

  std::cout << O_WRONLY << std::endl;

  return 0;
  char byte;
  int fFileDescriptor = open("/dev/cu.usbserial", O_RDWR);
  std::cout << fFileDescriptor << std::endl;
  write(fFileDescriptor, "A", 1);
  ssize_t size = read(fFileDescriptor, &byte, 1);
  printf("Read byte %c\n", byte);
  return 0;
}
*/

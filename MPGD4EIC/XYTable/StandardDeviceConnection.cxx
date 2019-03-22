#include <iostream>
#include <TString.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "StandardDeviceConnection.h"

ClassImp(StandardDeviceConnection);

//======
StandardDeviceConnection::StandardDeviceConnection() {
  fDevice = "/dev/NULL";
  fNULL = 0;
  fFileDescriptor = -1;
  fMode = O_RDWR;
}
//======
StandardDeviceConnection::StandardDeviceConnection(TString dev, Int_t mode) {
  fDevice = dev;
  fNULL = 0;
  fFileDescriptor = -1;
  fMode = mode;
  Init();
}
//======
StandardDeviceConnection::~StandardDeviceConnection() {
  if(fFileDescriptor<0) return;
  close(fFileDescriptor);
}
//======
void StandardDeviceConnection::Init() {
  fFileDescriptor = open(fDevice.Data(), fMode);
  if(fFileDescriptor<0) {
    std::cout << "StandardDeviceConnection::Init() cannot open device!" << std::endl;
    fNULL = 1;
  }
}
//======
void StandardDeviceConnection::Set7E1at4800() {
  if(fFileDescriptor<0) return;
  struct termios tty;
  if( tcgetattr(fFileDescriptor,&tty)<0 ) {
    std::cout << "SetBPS() error tcgetattr" << std::endl;
    return;
  }
  fcntl(fFileDescriptor,F_SETFL,FNDELAY);
  tty.c_cflag |= PARENB;
  tty.c_cflag &= ~PARODD;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS7;
  if( cfsetospeed(&tty, B4800 )<0 ) {
    std::cout << "SetBPS() error cfsetospeed" << std::endl;
    return;
  }
  if( cfsetispeed(&tty, B4800 )<0 ) {
    std::cout << "SetBPS() error cfsetispeed" << std::endl;
    return;
  }
  if( tcsetattr(fFileDescriptor,TCSANOW,&tty)<0 ) {
    std::cout << "SetBPS() error tcsetattr" << std::endl;
    return;
  }
  return;
}
//======
void StandardDeviceConnection::Send(TString word) {
  if(fFileDescriptor<0) return;
  ssize_t len = write(fFileDescriptor, word, word.Length());
  if(len!=word.Length()) {
    std::cout << "StandardDeviceConnection::Send() was interrupted!" << std::endl;
  }
}
//======
TString StandardDeviceConnection::Receive() {
  if(fFileDescriptor<0) return "";
  char word[1];
  ssize_t len = read(fFileDescriptor, word, 1);
  Int_t lent = len;
  if(lent<0) return "";
  if(lent!=1) {
    std::cout << "StandardDeviceConnection::Receive() was interrupted!" << std::endl;
    std::cout << "1 bytes requested" << std::endl;
    std::cout << lent << " ssize_t received" << std::endl;
    return "";
  }
  return word[0];
}
//======
void StandardDeviceConnection::Flush() {
  if(fFileDescriptor<0) return;
  //sleep(1);
  tcflush(fFileDescriptor,TCIOFLUSH);
}

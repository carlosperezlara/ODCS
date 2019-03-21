int pol() {
  TH2Poly *board = new TH2Poly("BOARD","BOARD",0,30,0,10);

  double x[100];
  double y[100];

  double gx = 0;
  for(int str=0; str!=26; ++str) {
    double xp = 1;
    double wd = 0.8*xp;
    double yp = 1;
    double st = 0.1;
    int nvert = (10/yp*2+1)*2;
    for(int i=0; i!=nvert; ++i) {
      double sx, sy;
      if(i<(nvert/2)) {
	sx = (i%2)*st;
	sy = i*yp/2;
      } else {
	sx = ((i+1)%2)*st;
	sy = (nvert-i-1)*yp/2;
      }
      sx += (i/(nvert/2))*wd;
      x[i] = gx + sx;
      y[i] = sy;
      //cout << x[i] << " | " << y[i] << endl;
    }
    board->AddBin(nvert,x,y);
    gx += xp;
  }
  board->Draw("col");
  return 0;
}

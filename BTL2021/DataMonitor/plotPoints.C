int plotPoints() {
  double x[] =  {5.6, 5.8, 6.0, 6.2, 6.4, 6.6, 6.8, 7.0, 7.5, 8.0, 8.5};
  double ex[] = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
  double y10[] = {1517, 1265,  763,  666,  512, 420, 380, 323, 283, 268, 223};
  double y20[] = {1102,  958,  800,  637,  613, 454, 432, 388, 346, 317, 283};
  double y30[] = {1199, 1045,  873,  756,  689, 522, 485, 480, 430, 393, 343};
  double y50[] = {1623, 1299, 1209, 1170, 1026, 734, 729, 719, 680, 660, 0};

  double ene0v[] = { 73.33, 93.92, 123.8, 158.2, 185.0, 252.1, 289.3, 356.6, 427.7, 506.7, 630.7};
  double ene1v[] = { 64.80, 83.87, 109.7, 139.3, 163.3, 221.4, 252.5, 309.0, 368.6, 438.3, 549.8};
  double ene0s[] = { 8.328, 8.973, 10.33, 11.83, 12.93, 16.85, 23.00, 23.94, 35.08, 46.57, 54.08};
  double ene1s[] = { 8.045, 8.768, 9.231, 11.31, 12.31, 15.54, 19.90, 21.37, 31.09, 42.45, 53.60};
  
  TGraph *gr10 = new TGraph(11,x,y10);
  TGraph *gr20 = new TGraph(11,x,y20);
  TGraph *gr30 = new TGraph(11,x,y30);
  TGraph *gr50 = new TGraph(10,x,y50);

  gr10->SetLineColor(kCyan-3);
  gr20->SetLineColor(kBlue-3);
  gr30->SetLineColor(kOrange-3);
  gr50->SetLineColor(kMagenta-3);

  gr10->SetMarkerColor(kCyan-3);
  gr20->SetMarkerColor(kBlue-3);
  gr30->SetMarkerColor(kOrange-3);
  gr50->SetMarkerColor(kMagenta-3);

  gr10->SetMarkerStyle(20);
  gr20->SetMarkerStyle(20);
  gr30->SetMarkerStyle(20);
  gr50->SetMarkerStyle(20);

  TGraphErrors *grE0 = new TGraphErrors(11,x,ene0v,ex,ene0s);
  TGraphErrors *grE1 = new TGraphErrors(11,x,ene1v,ex,ene1s);
  grE0->SetLineColor(kGray);
  grE1->SetLineColor(kBlack);
  grE0->SetMarkerColor(kGray);
  grE1->SetMarkerColor(kBlack);
  grE0->SetMarkerStyle(20);
  grE1->SetMarkerStyle(20);
  
  new TCanvas();
  gr10->Draw("APL");
  gr20->Draw("PLSAME");
  gr30->Draw("PLSAME");
  gr50->Draw("PLSAME");

  gr10->SetTitle("TIME");
  gr10->GetXaxis()->SetTitle("Laser Intensity   (a.u.)");
  gr10->GetYaxis()->SetTitle("#sigma  #Delta t   (ps)");
  TLegend *leg = new TLegend(0.75,0.62,0.9,0.9);
  leg->AddEntry( gr10, "thr_{10}" );
  leg->AddEntry( gr20, "thr_{20}" );
  leg->AddEntry( gr30, "thr_{30}" );
  leg->AddEntry( gr50, "thr_{50}" );
  leg->Draw();

  new TCanvas();
  grE0->Draw("APL");
  grE1->Draw("PLSAME");
  grE0->SetTitle("ENERGY");
  grE0->GetXaxis()->SetTitle("Laser Intensity   (a.u.)");
  grE0->GetYaxis()->SetTitle("Amplitude  LowGain  Channel   (mV)");
  TLegend *leg1 = new TLegend(0.1,0.62,0.3,0.9);
  leg1->AddEntry( grE0, "HDR  0" );
  leg1->AddEntry( grE1, "HDR  1" );
  leg1->Draw();
  
  return 0;
}

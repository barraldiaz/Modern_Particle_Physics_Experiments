#include <stdio.h>

#include <TROOT.h>

#include <TH1.h>
#include <TF1.h>
#include <TH1F.h>

#include <TCanvas.h>
#include <TStyle.h>

#include <TRandom3.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// ==============================================================================

using namespace std;
using std::cout;   

// ==============================================================================

// ROOT recommended random number generator with default seed.
// Mersenne Twister generator with period ~ 10^6000 
// TRandom2 (37 ns/call) is faster than TRandom3 (45 ns/call) but has a period ~ 10^26. 
// To improve speed we could utilise TRandom2 if the period is acceptable.

//UInt_t seed = 4357; // default root seed

UInt_t seed = 0;    // seed from TUUID (Universally Unique ID)

TRandom3 RND(seed);

// ==============================================================================

const long N_GEN_EVENTS = 10000;

const double MEAN_p = 2.0; // [GeV]

float p1,p2;
float the1,the2;
float phi1,phi2;

long N_EVENTS = 0;

vector<Float_t> P1;
vector<Float_t> P2;

vector<Float_t> THE1;
vector<Float_t> THE2;

vector<Int_t> TRG1;
vector<Int_t> TRG2;

//string REVISION = "Toy_MC_VER_TRACK_ACC_AND_TRG";
string REVISION = "Toy_MC_VER_ALL_GEN";

string pdf_name;

// ==============================================================================

// HISTOGRAMS: variable distributions (triggered/recorded)

Int_t N_bins_p = 50;

const Double_t MIN_p = 0.0;
const Double_t MAX_p = 5.0;

Int_t N_bins_the = 50;

const Double_t MIN_the = 0.0;
const Double_t MAX_the = 3.14159;

// ---

// HISTOGRAMS: variable distributions (accepted by trigger)

TH1F *h_p = new TH1F("h_p","h_p", N_bins_p,MIN_p,MAX_p);

// HISTOGRAMS: variable distributions (all generated)

TH1F *h_gen_p = new TH1F("h_gen_p","h_gen_p", N_bins_p,MIN_p,MAX_p);

// HISTOGRAMS: MC generated (true) efficiency

TH1F *h_gen_eff_p = new TH1F("h_gen_eff_p","h_gen_eff_p", N_bins_p,MIN_p,MAX_p);

// HISTOGRAMS: MC TAG'ed events

TH1F *h_TAG_p = new TH1F("h_TAG_p","h_TAG_p", N_bins_p,MIN_p,MAX_p);

// HISTOGRAMS: MC PROBE'ed events

TH1F *h_PROBE_p = new TH1F("h_PROBE_p","h_PROBE_p", N_bins_p,MIN_p,MAX_p);

// HISTOGRAMS: MC extracted (TAG and PROBE) efficiency

TH1F *h_eff_p = new TH1F("h_eff_p","h_eff_p", N_bins_p,MIN_p,MAX_p);

// ==============================================================================

double p_acceptance(double p) {

  double acc;

  if(p < 0.5) {

    acc = 0.0;

  } else if (p < 1.5) {

    acc = p - 0.5;
    
  } else {

    acc = 1.0;
    
  }

  return acc;
}

// ==============================================================================

double the_acceptance(double the) {

  double acc;

  acc = 0.8;

  if(the < 0.5)               acc = 0.0; // beam-pipe + tracker acceptance
  if(the > TMath::Pi() - 0.5) acc = 0.0; // beam-pipe + tracker acceptance
  
  return acc;
    
}

// ==============================================================================

void gener_MC(string file_name, long N) {

  fstream out_file;

  float p1,p2;
  float the1,the2;
  float phi1,phi2;

  float acc_p1,acc_p2;
  float acc_the1,acc_the2;

  float acc_1,acc_2;

  bool BOTH_IN_REGION_OF_ACCEPTANCE;
  
  int trg1,trg2;
  
  out_file.open(file_name, ios::out);

  for(int i_evt = 0; i_evt < N; i_evt++) {

    if((i_evt % 1000) == 0) {cout << "gener_MC: i_evt = " << i_evt << endl;}
    
    p1 = RND.Exp(MEAN_p);
    p2 = RND.Exp(MEAN_p);

    the1 = acos(RND.Uniform(2.0) - 1.0);
    the2 = acos(RND.Uniform(2.0) - 1.0);

    phi1 = RND.Uniform(2*TMath::Pi());
    phi2 = RND.Uniform(2*TMath::Pi());

    // calc acceptance using true values

    acc_p1 = p_acceptance(p1);
    acc_p2 = p_acceptance(p2);

    acc_the1 = the_acceptance(the1);
    acc_the2 = the_acceptance(the2);

    acc_1 = acc_p1*acc_the1;
    acc_2 = acc_p2*acc_the2;

    BOTH_IN_REGION_OF_ACCEPTANCE = (acc_1*acc_2 > 0.0);
      
    // generate trigger decision

    trg1 = 0;
    trg2 = 0;
    if(RND.Uniform(1.0) < acc_1) {trg1 = 1;} // mu1 fired
    if(RND.Uniform(1.0) < acc_2) {trg2 = 1;} // mu2 fired
    
    // smear the true values: make the simulation more realistic

    p1 = abs(RND.Gaus(p1,0.15));
    p2 = abs(RND.Gaus(p2,0.15));
    
    //the1 = abs(RND.Gaus(the1,0.05));
    //the2 = abs(RND.Gaus(the2,0.05));
    
    //out_file << p1 << " " << the1 << " " << phi1 << " " << trg1 << endl;
    //out_file << p2 << " " << the2 << " " << phi2 << " " << trg2 << endl;

    // if(BOTH_IN_REGION_OF_ACCEPTANCE && ((trg1 == 1) || (trg2 == 1))) {
    // if(BOTH_IN_REGION_OF_ACCEPTANCE) {
    if(true) {
    // if((trg1 == 1) || (trg2 == 1)) {
      out_file << p1 << " " << the1 << " " << trg1 << endl;
      out_file << p2 << " " << the2 << " " << trg2 << endl;
    }
    
  }
  
  out_file.close();
      
}

// ==============================================================================

void read_MC(string file_name, long& N) {

  fstream in_file;

  float p1,p2;
  float the1,the2;

  int trg1,trg2;
  
  in_file.open(file_name, ios::in);

  N = 0;
  while(in_file >> p1 >> the1 >> trg1 >> p2 >> the2 >> trg2) {

    //cout << p1 << " " << the1 << " " << trg1 << endl;
    //cout << p2 << " " << the2 << " " << trg2 << endl;

    P1.push_back(p1);
    P2.push_back(p2);

    THE1.push_back(the1);
    THE2.push_back(the2);
    
    TRG1.push_back(trg1);
    TRG2.push_back(trg2);
    
    N++;

    if((N % 1000) == 0) {cout << "read_MC: N = " << N << endl;}
    
  }
  
  in_file.close();

}

// ==============================================================================

void fill_gen_histos(long N) {

  float p1,p2;
  float the1,the2;

  int trg1,trg2;
  
  for(long i_evt = 0; i_evt < N; i_evt++) {

    p1 = P1[i_evt];
    p2 = P2[i_evt];

    the1 = THE1[i_evt];
    the2 = THE2[i_evt];

    trg1 = TRG1[i_evt];
    trg2 = TRG2[i_evt];

    // p
    
    h_gen_p->Fill(p1);
    h_gen_p->Fill(p2);

    // the
    
  }

  cout << "fill_gen_histos: done." << endl;
  
}

// ==============================================================================

void fill_trg_histos(long N) {

  float p1,p2;
  float the1,the2;

  int trg1,trg2;
  
  for(long i_evt = 0; i_evt < N; i_evt++) {

    p1 = P1[i_evt];
    p2 = P2[i_evt];

    the1 = THE1[i_evt];
    the2 = THE2[i_evt];

    trg1 = TRG1[i_evt];
    trg2 = TRG2[i_evt];

    // p
    
    if(trg1 == 1) h_p->Fill(p1);
    if(trg2 == 1) h_p->Fill(p2);

    // the

  }

  cout << "fill_trg_histos: done." << endl;
  
}

// ==============================================================================

void fill_TAG_and_PROBE_histos(long N) {

  float p1,p2;
  float the1,the2;

  int trg1,trg2;
  
  for(long i_evt = 0; i_evt < N; i_evt++) {

    p1 = P1[i_evt];
    p2 = P2[i_evt];

    the1 = THE1[i_evt];
    the2 = THE2[i_evt];

    trg1 = TRG1[i_evt];
    trg2 = TRG2[i_evt];

    // TAGs
    
    // p
    
    if(trg1 == 1) h_TAG_p->Fill(p2);
    if(trg2 == 1) h_TAG_p->Fill(p1);

    // the

    // TAGs and PROBEs
    
    // p
    
    if((trg1 == 1) && (trg2 == 1)) h_PROBE_p->Fill(p2);
    if((trg2 == 1) && (trg1 == 1)) h_PROBE_p->Fill(p1);

    // the

  }

  cout << "fill_TAG_and_PROBE_histos: done." << endl;
  
}

// ==============================================================================

void divide_histos() {

  // gen eff
  
  h_gen_eff_p->Divide(h_p, h_gen_p, 1.0, 1.0, "B");

  // TAG and PROBE

  h_eff_p->Divide(h_PROBE_p, h_TAG_p, 1.0, 1.0, "B");

  cout << "divide_histos: done." << endl;
}

// ==============================================================================

void plot_single_hist(TH1F* hist, TCanvas *canv, Int_t ipad, string title, string xlabel, string ylabel
		     ,string ylinlog, Double_t YMIN, Double_t YMAX) {

  canv->cd(ipad);
  TPad* pad = (TPad*) canv->GetPad(ipad);
  //pad->SetLeftMargin(0.13);
  //pad->SetRightMargin(0.04);
  pad->SetBottomMargin(0.2);

  hist->SetMinimum(YMIN);
  if(YMAX > 0.0) hist->SetMaximum(YMAX);
  if( (string) ylinlog == "ylog") {
    gPad->SetLogy(1);
  } else {
    gPad->SetLogy(0);
  }

  hist->GetXaxis()->SetLabelSize(0.08);
  hist->GetYaxis()->SetLabelSize(0.08);

  hist->GetXaxis()->SetTitleSize(0.08);
  hist->GetYaxis()->SetTitleSize(0.08);

  hist->GetYaxis()->SetTitleOffset(1.1);
  
  hist->SetTitle(title.c_str()); 
  hist->GetXaxis()->Delete();
  hist->GetXaxis()->SetTitle(xlabel.c_str());
  hist->GetYaxis()->Delete();
  hist->GetYaxis()->SetTitle(ylabel.c_str());
  hist->SetLineWidth(1);
  hist->SetLineColor(kBlack);
  hist->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  hist->SetFillColor(0);
  hist->SetFillStyle(0);
  hist->Draw("e0 hist");

}

// ==============================================================================

void plot_two_hist(TH1F* hist0, TH1F* hist1, TCanvas *canv, Int_t ipad, string title, string xlabel, string ylabel
		     ,string ylinlog, Double_t YMIN, Double_t YMAX) {

  // gStyle->SetOptStat(0);
  
  canv->cd(ipad);
  TPad* pad = (TPad*) canv->GetPad(ipad);
  //pad->SetLeftMargin(0.13);
  //pad->SetRightMargin(0.04);
  pad->SetBottomMargin(0.2);
  
  hist0->SetMinimum(YMIN);
  if(YMAX > 0.0) hist0->SetMaximum(YMAX);
  if( (string) ylinlog == "ylog") {
    gPad->SetLogy(1);
  } else {
    gPad->SetLogy(0);
  }

  hist0->GetXaxis()->SetLabelSize(0.08);
  hist0->GetYaxis()->SetLabelSize(0.08);

  hist0->GetXaxis()->SetTitleSize(0.08);
  hist0->GetYaxis()->SetTitleSize(0.08);

  hist0->GetYaxis()->SetTitleOffset(1.1);
  
  hist0->SetTitle(title.c_str()); 
  hist0->GetXaxis()->Delete();
  hist0->GetXaxis()->SetTitle(xlabel.c_str());
  hist0->GetYaxis()->Delete();
  hist0->GetYaxis()->SetTitle(ylabel.c_str());
  hist0->SetLineWidth(1);
  hist0->SetLineColor(kBlack);
  hist0->SetMarkerColor(kBlack);
  hist0->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  hist0->Draw("e0 hist");
  
  hist1->SetTitle(title.c_str()); 
  hist1->GetXaxis()->Delete();
  hist1->GetXaxis()->SetTitle(xlabel.c_str());
  hist1->GetYaxis()->Delete();
  hist1->GetYaxis()->SetTitle(ylabel.c_str());
  hist1->SetLineWidth(1);
  hist1->SetLineColor(kRed);
  hist1->SetMarkerColor(kRed);
  hist1->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  hist1->Draw("e0 hist same");

  // gStyle->SetOptStat(1); 
  
}

// ==============================================================================

void plot_all_histos() {

  gROOT->Reset();
  gROOT->Clear();

  gStyle->SetOptTitle(1);
  // gStyle->SetOptStat(); // now in ~/.rootrcdir/rootlogon.C
  // gStyle->SetOptFit(1);

  //gStyle->SetOptStat(1000000000);
  gStyle->SetOptStat(0);
  
  gStyle->SetMarkerStyle(kFullCircle);
  gStyle->SetMarkerSize(0.75); // 1.0 = 8 pixels

  TCanvas *canv1 = new TCanvas("canv1","canv1",10,10,1400,700);

  Int_t ipad = 0;

  // ---
  
  //
  // p: h_gen_eff plus TAG and PROBE effic
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);

  plot_two_hist(h_gen_eff_p, h_eff_p , canv1, 1, "p eff", "p (GeV)", "eff", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = REVISION+"_p_gen_eff_plus_TAG_and_PROBE_eff.pdf";
  canv1->Print(pdf_name.c_str());
  
  //
  // p gen and trg
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);

  plot_two_hist(h_gen_p, h_p, canv1, 1, "p gen and trg", "p (GeV)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = REVISION+"_p_gen_trg_lin.pdf";
  canv1->Print(pdf_name.c_str());

  //
  // p TAG and PROBE
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);

  plot_two_hist(h_TAG_p, h_PROBE_p, canv1, 1, "p TAG and PROBE", "p (GeV)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = REVISION+"_p_TAG_and_PROBE_lin.pdf";
  canv1->Print(pdf_name.c_str());

  // ---
  
  cout << endl;
  cout << "plot_all_histos: done." << endl;
  
}

// ==============================================================================

void Trig_eff_toy_mc() {

  cout << endl;
  cout << "Trig_eff_toy_mc: start..." << endl;
  cout << endl;

  gener_MC("Trig_eff_toy_mc.txt", N_GEN_EVENTS);

  read_MC("Trig_eff_toy_mc.txt", N_EVENTS);

  cout << endl;
  cout << "N_EVENTS = " << N_EVENTS << endl;

  fill_gen_histos(N_EVENTS);

  fill_trg_histos(N_EVENTS);

  fill_TAG_and_PROBE_histos(N_EVENTS);
  
  divide_histos();
  
  plot_all_histos();
  
  cout << endl;
  cout << "Trig_eff_toy_mc: THE END." << endl;
  
}

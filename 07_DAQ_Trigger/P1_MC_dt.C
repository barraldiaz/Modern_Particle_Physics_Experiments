#include <stdio.h>

#include <TROOT.h>

#include <TH1.h>
#include <TF1.h>
#include <TH1F.h>

#include <TCanvas.h>
#include <TStyle.h>

#include <TRandom3.h>

#include <TString.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// ==============================================================================

using namespace std;
using std::cout;   

// ==============================================================================

string revision = "VER_MC";

string pdf_name;

// ==============================================================================

// MC parameters

const Int_t N_MC_EVENTS_GEN = 100000;

const Double_t TAU   = 103.8; // [ms]
const Double_t DEAD_TIME = 220.0; // [ms]

// MC DST structure

Int_t N_MC_EVENTS   = 0; // number of MC events accepted outside dead-time

//vector<Double_t> MC_EVT; // the same for MC, Poissonian model
//vector<Double_t> MC_TRG;
vector<Double_t> MC_TIME; // [ms]

// fitted variables

Double_t N_evt;
Double_t tau;

Double_t N_evt_err;
Double_t tau_err;

// ==============================================================================

// book the histograms

// --- time between events

Int_t N_bins_Dt = 100;

const Double_t X_MIN_Dt =    0.0;
const Double_t X_MAX_Dt = 1000.0; // [ms]

TH1F *h_MC_Dt = new TH1F("h_MC_Dt","h_MC_Dt"               ,N_bins_Dt,X_MIN_Dt,X_MAX_Dt);
TH1F *h_MC_FULL_Dt = new TH1F("h_MC_FULL_Dt","h_MC_FULL_Dt",N_bins_Dt,X_MIN_Dt,X_MAX_Dt);

// ==============================================================================

// ROOT recommended random number generator with default seed.
// Mersenne Twister generator with period ~ 10^6000 
// TRandom2 (37 ns/call) is faster than TRandom3 (45 ns/call) but has a period ~ 10^26. 
// To improve speed we could utilise TRandom2 if the period is acceptable.

//UInt_t seed = 4357; // default root seed

UInt_t seed = 0;    // seed from TUUID (Universally Unique ID)

TRandom3 RND(seed);

// ==============================================================================

// Users functions

// ==============================================================================

void gen_MC(Int_t N_gen, Int_t &N_acc, Double_t tau, Double_t dead_time) {

  Double_t dt;
  Double_t dt_sum  = DEAD_TIME; // time spend until the end of dead time gate, first event is always accepted

  Double_t exposure_time = 0.0; // exposure time = total elapsed time
  Double_t i_exposure_time; // as above as long int

  Double_t tot_dead_time; // total dead time = sum of all dead time gates
  Double_t dead_time_frac;

  Int_t i_acc = 0;
  
  // in DATA we count the events from 1, do the same in MC
  for(int i_gen = 1; i_gen <= N_gen; i_gen++) {

    if((i_gen % 1000) == 0) cout << "gen_MC: i_gen, i_acc = " << i_gen << "  " << i_acc << endl;
    
    dt = RND.Exp(tau); // generate time to next event from Exp() distribution
    dt_sum = dt_sum + dt; // time spend in current dead time gate
    exposure_time = exposure_time + dt; // total exposure time = total elapsed time

    i_exposure_time = exposure_time; // truncate it to integer as in DATA [ms]

    h_MC_FULL_Dt->Fill(dt_sum);
    
    if(dt_sum >= dead_time) {
      // accept this event
      i_acc++;
      MC_TIME.push_back(i_exposure_time);
      dt_sum = 0.0; // start new dead time gate
    }
  }

  N_acc = i_acc;

  // calculate dead time fraction (two methods)
  
  tot_dead_time = N_acc*dead_time;
  dead_time_frac = tot_dead_time/exposure_time;

  cout << endl;
  cout << "gen_MC: N_gen = " << N_gen << endl;
  cout << "gen_MC: N_acc = " << N_acc << endl;
  cout << "gen_MC: i_exposure_time [ms] = " << i_exposure_time << endl;
  cout << endl;
  cout << "gen_MC: tau       = " << tau << endl;
  cout << "gen_MC: dead_time = " << dead_time << endl;
  cout << endl;
  cout << "gen_MC: dead_time_frac  = " << dead_time_frac << endl;
  cout << "gen_MC: 1 - N_acc/N_gen = " << 1.0 - (Double_t)N_acc/(Double_t)N_gen << endl;
  cout << endl;
  
}

// ==============================================================================

void fill_TIMING_histos(TH1F* h_Dt, Int_t N_EVENTS, vector<Double_t> iTIME) {

  Double_t dt;

  // start from 1 to count the first dt !
  for(int i = 1; i < N_EVENTS; i++) {

    dt = (iTIME[i] - iTIME[i-1]);

    h_Dt->Fill(dt);

  }

}

// ==============================================================================

void plot_single_hist(TH1F* hist, TCanvas *canv, Int_t ipad, string title, string xlabel, string ylabel
		     ,string ylinlog, Double_t YMIN, Double_t YMAX) {

  canv->cd(ipad);

  hist->SetMinimum(YMIN);
  if(YMAX > 0.0) hist->SetMaximum(YMAX);
  if( (string) ylinlog == "ylog") {
    gPad->SetLogy(1);
  } else {
    gPad->SetLogy(0);
  }

  hist->SetTitle(title.c_str()); 
  hist->GetXaxis()->Delete();
  hist->GetXaxis()->SetTitle(xlabel.c_str());
  hist->GetYaxis()->Delete();
  hist->GetYaxis()->SetTitle(ylabel.c_str());
  hist->SetLineWidth(1);
  hist->SetLineColor(kBlack);
  hist->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  hist->Draw("e0 hist");

}

// ==============================================================================

void fit_and_plot_single_Exp_hist(TH1F* hist, Double_t FIT_start, Double_t FIT_stop,
 				        Double_t &N,    Double_t &N_err,
				        Double_t &tau,  Double_t &tau_err,
				        TCanvas *canv, Int_t ipad, string title, string xlabel, string ylabel,
                                        string ylinlog, Double_t YMIN, Double_t YMAX) {

  Double_t XMIN = FIT_start;
  Double_t XMAX = FIT_stop;

  Double_t XEPS = 4.0; // [ms]
  
  // ---

  string h_name = hist->GetName();
  
  // prepare function to be fitted

  string s_func_exp;
  string s_func_const;
  
  // our histos have equidistant bins, bin number "1" below is irrelevant...

  Double_t bin_w = hist->GetXaxis()->GetBinWidth(1);

  ostringstream oss_bin_w; 
  string str_bin_w;

  oss_bin_w << bin_w;
  str_bin_w = oss_bin_w.str();

  s_func_exp = str_bin_w + (string)"*([0]/[1]*exp(-x/[1]))";
  
  s_func_const = str_bin_w + (string)"*([0]/[1]*exp(-[2]/[1]))";
  
  // ---

  TF1 *f_func_exp    = new TF1("Exp"     ,s_func_exp.c_str()    ,XMIN-XEPS, XMAX);

  TF1 *f_func_const    = new TF1("Const"     ,s_func_const.c_str()    ,0.0, XMIN);
  
  //
  // Initialize parameters
  //

  // Exp

  f_func_exp->SetParName(0,"N");
  f_func_exp->SetParameter(0,N);
  // f_func->SetParLimits(0,N - N/5,N + N/5);

  f_func_exp->SetParName(1,"tau");
  f_func_exp->SetParameter(1,tau);
  // f_func->SetParLimits(1,tau-0.5,rms+0.5);

  // ---

  cout << endl;
  cout << "fit_and_plot_single_Exp_hist: ->Fit(...), hist name = " << h_name << endl;
  cout << endl;
  
  hist->Fit("Exp","0L","",XMIN,XMAX);
  
  // --- export some fitted parameters

  N     = f_func_exp->GetParameter(0);
  tau   = f_func_exp->GetParameter(1);

  N_err     = f_func_exp->GetParError(0);
  tau_err   = f_func_exp->GetParError(1);

  cout << endl;
  cout << "fit_and_plot_single_Exp_hist: " << h_name << ": N   = " << N << endl;
  cout << "fit_and_plot_single_Exp_hist: " << h_name << ": tau = " << tau << endl;
  cout << endl;
  cout << "fit_and_plot_single_Exp_hist: " << h_name << ": FIT_start = " << FIT_start << endl;
  cout << "fit_and_plot_single_Exp_hist: " << h_name << ": FIT_stop  = " << FIT_stop << endl;
  cout << endl;

  // set parameters for const function

  f_func_const->SetParameter(0,N);
  f_func_const->SetParameter(1,tau);
  f_func_const->SetParameter(2,FIT_start); // ie.: DEAD_TIME
  
  // calculate fraction of dead time

  Double_t naive_f_td;
  Double_t proper_f_td;

  naive_f_td = 1.0 - exp(-FIT_start/tau);
  proper_f_td = FIT_start/(tau + FIT_start);
  
  cout << endl;
  cout << "fit_and_plot_single_Exp_hist: " << h_name << ": naive_f_td  = " << naive_f_td << endl;
  cout << "fit_and_plot_single_Exp_hist: " << h_name << ": proper_f_td = " << proper_f_td << endl;
  cout << endl;
  
  // --- plot histos and fitted function

  canv->cd(ipad);

  hist->SetMinimum(YMIN);
  if(YMAX > 0.0) hist->SetMaximum(YMAX);
  if( (string) ylinlog == "ylog") {
    gPad->SetLogy(1);
  } else {
    gPad->SetLogy(0);
  }
  
  // ---

  //
  // plot histogram
  //
  
  hist->SetTitle(title.c_str()); 
  hist->GetXaxis()->Delete();
  hist->GetXaxis()->SetTitle(xlabel.c_str());
  hist->GetYaxis()->Delete();
  hist->GetYaxis()->SetTitle(ylabel.c_str());
  hist->SetLineWidth(1);
  hist->SetLineColor(kBlack);
  hist->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  hist->Draw("ex0");

  //
  // plot fitted function
  //
  
  f_func_exp->SetLineColor(kRed);
  f_func_exp->SetLineWidth(1);
  f_func_exp->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  f_func_exp->Draw("same");

  f_func_const->SetLineColor(kRed);
  f_func_const->SetFillColor(kRed);
  f_func_const->SetLineWidth(2);
  f_func_const->SetLineStyle(1); // 1 == solid, 2 == dashed, 3 == dotted
  f_func_const->SetFillStyle(3344); // 1 == solid, 2 == dashed, 3 == dotted
  f_func_const->Draw("same");
  
  // plot once again DATA to have 'bullets' on top of histos and fitted functions

  hist->Draw("ex0 same");
  f_func_exp->Draw("same");
  f_func_const->Draw("same");
  
}

// ==============================================================================

void fit_and_plot_all_histos() {

  gROOT->Reset();
  gROOT->Clear();

  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);
  
  gStyle->SetMarkerStyle(kFullCircle);
  gStyle->SetMarkerSize(0.75); // 1.0 = 8 pixels

  TCanvas *canv1 = new TCanvas("canv1","canv1",10,10,1400,700);

  Int_t ipad = 0;

  //
  // MC plots
  //
  
  //
  // --- h_MC_Dt lin
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);


  N_evt = 20000.0;
  tau   =   100.0;
  
  plot_single_hist(h_MC_Dt, canv1, 1, "Dt", "(ms)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = "P1_"+revision+"_MC_Dt_lin.pdf";
  canv1->Print(pdf_name.c_str());

  //
  // --- fitted Exp to h_MC_Dt lin
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);


  N_evt = 20000.0;
  tau   =   100.0;
  
  fit_and_plot_single_Exp_hist(h_MC_Dt, DEAD_TIME, X_MAX_Dt,
			                N_evt, N_evt_err,
			                tau, tau_err,
			                canv1, 1, "Exp", "(ms)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = "P1_"+revision+"_FIT_MC_Dt_lin.pdf";
  canv1->Print(pdf_name.c_str());

  //
  // --- h_MC_FULL_Dt lin
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);


  N_evt = 20000.0;
  tau   =   100.0;
  
  plot_single_hist(h_MC_FULL_Dt, canv1, 1, "Dt FULL", "(ms)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = "P1_"+revision+"_MC_FULL_Dt_lin.pdf";
  canv1->Print(pdf_name.c_str());

  //
  // --- fitted Exp to h_MC_FULL_Dt lin
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);


  N_evt = 20000.0;
  tau   =   100.0;
  
  fit_and_plot_single_Exp_hist(h_MC_FULL_Dt, DEAD_TIME, X_MAX_Dt,
			                N_evt, N_evt_err,
			                tau, tau_err,
			                canv1, 1, "N*Exp(-t/tau)", "#Delta t (ms)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = "P1_"+revision+"_FIT_MC_FULL_Dt_lin.pdf";
  canv1->Print(pdf_name.c_str());

}

// ==============================================================================

void P1_MC_dt() {

// ------------------------------------------------------------------------------
  
  cout << endl;
  cout << "P1_MC_dt: start..." << endl;
  cout << "P1_MC_dt: revision = " << revision << endl;
  cout << endl;

// ------------------------------------------------------------------------------

  gen_MC(N_MC_EVENTS_GEN, N_MC_EVENTS, TAU, DEAD_TIME);

  cout << endl;
  cout << "P1_MC_dt: N_MC_EVENTS   = " << N_MC_EVENTS << endl;
  
// ------------------------------------------------------------------------------
  
  fill_TIMING_histos(h_MC_Dt, N_MC_EVENTS, MC_TIME);

// ------------------------------------------------------------------------------
  
  fit_and_plot_all_histos();

// ------------------------------------------------------------------------------
  
  cout << endl;
  cout << "P1_MC_dt: THE END." << endl;
  
}

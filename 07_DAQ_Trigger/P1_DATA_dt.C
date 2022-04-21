#include <stdio.h>

#include <TROOT.h>

#include <TH1.h>
#include <TF1.h>
#include <TH1F.h>

#include <TCanvas.h>
#include <TStyle.h>

#include <TString.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// ==============================================================================

using namespace std;
using std::cout;   

// ==============================================================================

// DATA input directory and file name

string INP_DIR  = "./"; // current directory

//string INP_FILE = "P1_DST_ext_trg_AD.p1t"; // trigger on external planes AD
string INP_FILE = "P1_DST_int_trg_BC.p1t"; // trigger on internal planes BC

string revision = "VER_BC";

string pdf_name; // output graphics file

// ==============================================================================

// DST structure

const int N_COL = 39; // ALL columns
const int N_iCOL = 3; // int info/header

Int_t N_DATA_EVENTS = 0; // number of DATA events as read from DST

vector< vector<Double_t> > DST; // full DST, 2D vector: including amplitudes, noises and quality info

vector<Double_t> DST_EVT; // DST header info only
vector<Double_t> DST_TRG;
vector<Double_t> DST_TIME; // [ms]

vector<Double_t> DST_A1;
vector<Double_t> DST_A2;
vector<Double_t> DST_A3;

vector<Double_t> DST_B1;
vector<Double_t> DST_B2;
vector<Double_t> DST_B3;

vector<Double_t> DST_C1;
vector<Double_t> DST_C2;
vector<Double_t> DST_C3;

vector<Double_t> DST_D1;
vector<Double_t> DST_D2;
vector<Double_t> DST_D3;

// fitted variables (for exponential fit)

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

TH1F *h_DATA_Dt = new TH1F("h_DATA_Dt","h_DATA_Dt", N_bins_Dt, X_MIN_Dt, X_MAX_Dt);

// ==============================================================================

// Users functions

// ==============================================================================

void read_DST(int &N_evt_read) {

    fstream in_file;

    int n_lines = 0;
    int n_items = 0;
    int i_col = 0;

    string s_item;
    Double_t  f_item;
    long   i_item;
    
    vector<Double_t> ROW; // single ROW from DST
    vector<long> iROW;

    ROW.clear();
    iROW.clear();
    
    in_file.open(INP_DIR+INP_FILE, ios::in);

    if(!in_file){
      cout << "read_DST: ERROR: cannot open input file = " << INP_DIR+INP_FILE << endl;
      exit(0);
    }
    cout << endl;

    // parse DST file
    while(in_file >> s_item) {

      // cout << "s_item = " << s_item << endl;

      // convert input to long and double
      istringstream iss_f_item(s_item);
      istringstream iss_i_item(s_item);

      iss_f_item >> f_item;
      iss_i_item >> i_item;
      
      ROW.push_back(f_item);

      i_col = n_items % N_COL;

      if(i_col < N_iCOL) {
	iROW.push_back(i_item);
      }
      
      n_items++;

      if((n_items % N_COL) == 0) {
	// new line was completed
	DST.push_back(ROW);

	DST_EVT.push_back(iROW[0]);
	DST_TRG.push_back(iROW[1]);
	DST_TIME.push_back(iROW[2]);

	DST_A1.push_back(ROW[3]);
	//DST_A2.push_back(ROW[6]);
	DST_A2.push_back(ROW[3]+ROW[9]); // use Left+Right sum as total charge
	DST_A3.push_back(ROW[9]);
	
	DST_B1.push_back(ROW[12]);
	//DST_B2.push_back(ROW[15]);
	DST_B2.push_back(ROW[12]+ROW[18]);
	DST_B3.push_back(ROW[18]);
	
	DST_C1.push_back(ROW[21]);
	//DST_C2.push_back(ROW[24]);
	DST_C2.push_back(ROW[21]+ROW[27]);
	DST_C3.push_back(ROW[27]);
	
	DST_D1.push_back(ROW[30]);
	//DST_D2.push_back(ROW[33]);
	DST_D2.push_back(ROW[30]+ROW[36]);
	DST_D3.push_back(ROW[36]);
	
	ROW.clear();
	iROW.clear();
	n_lines++;
	if(n_lines % 1000 == 0) cout << "read_DST: n_lines read so far = " << n_lines << endl;
      }
    }

    N_evt_read = n_lines;

    in_file.close();
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

  // ---

  string h_name = hist->GetName();
  
  // prepare function to be fitted

  string s_func_exp;
  
  // our histos have equidistant bins, bin number "1" below is "any bin"

  Double_t bin_w = hist->GetXaxis()->GetBinWidth(1);

  ostringstream oss_bin_w; 
  string str_bin_w;

  oss_bin_w << bin_w;
  str_bin_w = oss_bin_w.str();

  // multiply by bin width to express N as "number of events"
  s_func_exp = str_bin_w + (string)"*([0]/[1]*exp(-x/[1]))";
  
  // ---

  TF1 *f_func_exp    = new TF1("Exp", s_func_exp.c_str(), XMIN, XMAX);

  //
  // Initialize parameters
  //

  // Exp

  f_func_exp->SetParName(0,"N");
  f_func_exp->SetParameter(0,N);

  f_func_exp->SetParName(1,"tau");
  f_func_exp->SetParameter(1,tau);

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

  // plot once again DATA to have 'bullets' on top of histos and fitted functions

  hist->Draw("ex0 same");
  // f_func_exp->Draw("same");

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
  //  P1 RAW data
  //
  
  //
  // --- fitted Exp to DATA_Dt lin
  //
  
  canv1->Clear();
  canv1->Divide(1,1,0.005,0.005);

  // ---

  N_evt = 20000.0;
  tau   =   100.0;
  
  fit_and_plot_single_Exp_hist(h_DATA_Dt, 220.0, X_MAX_Dt,
			                N_evt, N_evt_err,
			                tau, tau_err,
			                canv1, 1, "DATA: N*Exp(-t/tau)", "#Delta t (ms)", "Nevents", "ylin", 0.0, 0.0);

  canv1->Update();
  pdf_name = "P1_"+revision+"_FIT_DATA_Dt_lin.pdf";
  canv1->Print(pdf_name.c_str());

}

// ==============================================================================

void P1_DATA_dt() {

// ------------------------------------------------------------------------------
  
  cout << endl;
  cout << "P1_DATA_dt: start..." << endl;
  cout << "P1_DATA_dt: revision = " << revision << endl;
  cout << endl;

// ------------------------------------------------------------------------------

  read_DST(N_DATA_EVENTS);

  cout << endl;
  cout << "P1_DATA_dt: N_DATA_EVENTS = " << N_DATA_EVENTS << endl;
  
// ------------------------------------------------------------------------------
    
  fill_TIMING_histos(h_DATA_Dt, N_DATA_EVENTS, DST_TIME);

// ------------------------------------------------------------------------------
  
  fit_and_plot_all_histos();

// ------------------------------------------------------------------------------
  
  cout << endl;
  cout << "P1_DATA_dt: THE END." << endl;

// ------------------------------------------------------------------------------
  
}

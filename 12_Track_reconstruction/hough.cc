#include<iostream>
#include<cmath>
#include <TH2D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include  <TApplication.h>
#include  <TStyle.h>
#include  <TLatex.h>
#include <string>
#include <cstdlib>

using namespace std;


void prepare_data(string option, int nnoise);
void style();
// Function from official ROOT example, needed if you want to have
// active ROOT windows
void StandaloneApplication(int argc, char** argv);

//-----------------------------------------------------------------------
// Test of Hough transform for 5 straight lines (+ random noise)
//-----------------------------------------------------------------------
int main(int argc, char** argv){

  srandom(time(0));
  
// TApplication & StandaloneApplication  from official ROOT example,
// needed if you want to have active ROOT windows 
  
  TApplication app("ROOT Application", &argc, argv);
  StandaloneApplication(app.Argc(), app.Argv());
  app.Run();
 
  return 0;
}

//-----------------------------------------------------------------------
// Prepare input points and produce final plots
//-----------------------------------------------------------------------
void prepare_data(string option, int nnoise){

  // Define your drawing style
  style();

  // Prepare input to Hough algorithm: points along five straight lines
  int npoints=20;
  
  double x[npoints];
  double y1[npoints];
  double y2[npoints];  
  double y3[npoints];
  double y4[npoints];
  double y5[npoints];

  double j=0;
  for(int i=0; i<npoints; i++){
    x[i]=j;
    j=j+0.5;
    y1[i]=0.5*x[i]+2;
    y2[i]=0.3*x[i]+3;
    y3[i]=1.5*x[i]-2;
    y4[i]=1.5*x[i]-3;
    y5[i]=2*x[i];      
  }
  // Add random noise points

  double xn[nnoise];
  double yn[nnoise];
  
  if(nnoise>0){
    for(int i=0; i<nnoise; i++){
      xn[i]=10*(double)rand()/RAND_MAX;
      yn[i]=-4+24*(double)rand()/RAND_MAX;      
    }
  }
  // Plot data points
  if(option=="input"){ 

    TCanvas *canvas_input = new TCanvas("canvas_data","Input",150,10,700,700);  
    canvas_input->Clear();
    canvas_input->Divide(2,2,0.005,0.005);
    
    // Has to be before TGraph
    gStyle->SetMarkerStyle(7);
  
    TGraph *prosta1 = new TGraph(npoints, x, y1);
    TGraph *prosta2 = new TGraph(npoints, x, y2);
    TGraph *prosta3 = new TGraph(npoints, x, y3);  
    TGraph *prosta4 = new TGraph(npoints, x, y4);
    TGraph *prosta5 = new TGraph(npoints, x, y5);
    TGraph *noise = new TGraph(nnoise, xn, yn);    

    
    prosta1->SetMarkerColor(kBlue); 
    prosta2->SetMarkerColor(kBlue);
    prosta3->SetMarkerColor(kBlue);
    prosta4->SetMarkerColor(kBlue);
    prosta5->SetMarkerColor(kBlue);
    noise->SetMarkerColor(kBlue);    
  
    TMultiGraph *proste = new TMultiGraph();
    proste->Add(prosta1);
    proste->Add(prosta2);
    proste->Add(prosta3);
    proste->Add(prosta4);
    proste->Add(prosta5);  
    if(nnoise>0){
      proste->Add(noise);
    }
    proste->GetXaxis()->SetTitle("x");
    proste->GetYaxis()->SetTitle("y");
    proste->Draw("ap");
    
    canvas_input->Update();
    gPad->Modified();
    gPad->Update();
    
    //canvas_input->Print("hough_input.pdf");

  }
  // Plot Hough r -theta space
  else if(option=="hspace"){
  
  int range=180;
  int nbinsx=2*range;
  int nbinsy=100;

  TH2D * ho1 = new TH2D("ho1","Hough 1 track ",nbinsx,90.,180.,nbinsy,-5.,5.);
  TH2D * ho2 = new TH2D("ho2","Hough track 2",nbinsx,90.,180.,nbinsy,-5.,5.);
  TH2D * ho3 = new TH2D("ho3","Hough track 3",nbinsx,90.,180.,nbinsy,-5.,5.);
  TH2D * ho4 = new TH2D("ho4","Hough track 4",nbinsx,90.,180.,nbinsy,-5.,5.);
  TH2D * ho5 = new TH2D("ho5","Hough track 5",nbinsx,90.,180.,nbinsy,-5.,5.);
  TH2D * ho = new TH2D("ho","Hough all tracks",nbinsx,90.,180.,nbinsy,-5.,5.);
  TH2D * no = new TH2D("no","Hough noise",nbinsx,90.,180.,nbinsy,-5.,5.);  
  

  //
  // Number of lines (sinusoids) in the theta-r plot
  // is equal to the number of points along the track  
  //
  double theta,r1,r2,r3,r4,r5,rn;
  // Loop over all points belonging to  
  for(int i=0; i<npoints;i++){
    double ith=0.;
    double ang_width=0.5;
    // Form lines going through each point
    while(ith<=range-(ang_width)){ 
      theta=(M_PI/180.)*(ith);
      ith=ith+ang_width;
      r1=x[i]*cos(theta)+y1[i]*sin(theta);
      r2=x[i]*cos(theta)+y2[i]*sin(theta);
      r3=x[i]*cos(theta)+y3[i]*sin(theta);
      r4=x[i]*cos(theta)+y4[i]*sin(theta);
      r5=x[i]*cos(theta)+y5[i]*sin(theta);      

      ho1->Fill(ith,r1);
      ho2->Fill(ith,r2);
      ho3->Fill(ith,r3);
      ho4->Fill(ith,r4);            
      ho5->Fill(ith,r5);

      // All lines in one 2D histogram
      ho->Fill(ith,r1);
      ho->Fill(ith,r2);
      ho->Fill(ith,r3);
      ho->Fill(ith,r4);            
      ho->Fill(ith,r5);      

    }
    cout << endl;
  }

  // If there is noise:
  for(int i=0; i<nnoise;i++){
    double ith=0.;
    double ang_width=0.5;
    // Form lines going through each point
    while(ith<=range-(ang_width)){ 
      theta=(M_PI/180.)*(ith);
      ith=ith+ang_width;
      rn=xn[i]*cos(theta)+yn[i]*sin(theta);

      no->Fill(ith,rn);
      ho->Fill(ith,rn);
    }
    cout << endl;
  }

  TCanvas *canvas_hough = new TCanvas("canvas_hough","theta-r Hough space",150,10,700,700);  

  canvas_hough->Clear();
  canvas_hough->Divide(2,2,0.005,0.005);
   
  canvas_hough->cd(1);
  {
    ho1->GetXaxis()->SetTitle("#theta");
    ho1->GetYaxis()->SetTitle("r");
    ho1->Draw(); 
  }
  canvas_hough->cd(2);
  {
    ho->GetXaxis()->SetTitle("#theta");
    ho->GetYaxis()->SetTitle("r");
    ho->Draw(); 
  }

  canvas_hough->cd(3);
  {
    ho1->GetXaxis()->SetTitle("#theta");
    ho1->GetYaxis()->SetTitle("r");
    ho1->Draw("lego"); 
  }
  canvas_hough->cd(4);
  {
    ho->GetXaxis()->SetTitle("#theta");
    ho->GetYaxis()->SetTitle("r");
    ho->Draw("lego");
  }

  canvas_hough->Update();
  //canvas_hough->Print("hough_space.pdf");
  }
}
//-----------------------------------------------------------------------
// Function from official ROOT example, needed if you want to have
// active ROOT windows
//-----------------------------------------------------------------------
void StandaloneApplication(int argc, char** argv) {
  string option;
  // number of random noise points
  int nnoise=0;
  
  // arguments of the main function:
  if(argc==1){
    cout << "Not enough arguments of the main function" << endl;
    cout << "Should be:" << endl;
    cout << "./hough type_of_action" << endl;
    cout << "or" << endl;
    cout << "./hough type_of_action nb_of_random_noise_points" << endl;   
    cout << "Examples: " << endl;
    cout << "1) Plot input points without noise: " << endl;
    cout << "./hough input" << endl;
    cout << "2) Plot r-theta Hough space without noise: " << endl;
    cout << "./hough hspace" << endl;
    cout << "3) Plot input points with 10 noise points: " << endl;    
    cout << "./hough input 10" << endl;
    cout << "4) Plot r-theta Hough space with 10 noise points:" << endl;    
    cout << "./hough hspace 10" << endl;        
    exit(EXIT_FAILURE);
  }
  else if(argc==2){
    option=(string)argv[1];    
  }
  else if(argc==3){
    option=(string)argv[1];
    nnoise=atoi(argv[2]);
  }
    
  prepare_data(option,nnoise);
  //draw();
}

//-----------------------------------------------------------------------
// Define the style of your plots
//-----------------------------------------------------------------------
void style(){

  
  gStyle->SetOptStat(0); // no statistics
  gStyle->SetCanvasColor(kWhite);     // background is no longer mouse-dropping
  // 
  // gStyle->SetOptTitle(0); // no title
  gStyle->SetTitleSize(0.05,"xy"); 
    gStyle->SetTitleOffset(1.2,"x");
  gStyle->SetTitleOffset(1.0,"y");

  //gStyle->SetTextSize(1.1);
  //gStyle->SetLabelSize(0.05,"xy");
  gStyle->SetTitleSize(0.05,"xy");
  gStyle->SetTitleOffset(1.0,"x");
  gStyle->SetTitleOffset(1.0,"y");
  gStyle->SetPadTopMargin(0.12);
  gStyle->SetPadRightMargin(0.12);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.11);
    
}

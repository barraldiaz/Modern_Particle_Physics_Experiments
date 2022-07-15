/// This macro provides a simple example on how to use the trained classifier
/// within an analysis module

#include <TFile.h>
#include <TString.h>
#include <TSystem.h>
#include <TROOT.h>

#include <TMVA/Tools.h>
#include <TMVA/Reader.h>
#include <TMVA/MethodCuts.h>

#include<fstream>
#include<string>

using namespace TMVA;

int main(){

   // Create the Reader object
  
     TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

     // Create a set of variables and declare them to the reader
     // - the variable names MUST corresponds in name and type
     // to those given in the weight file(s) used
      
     // The order of variables is important ! 
     
     Float_t var1;
     Float_t var2;

     reader->AddVariable( "var1", &var1 );
     reader->AddVariable( "var2", &var2 );

     // --- Location of training files

     TString dir    = "dataset/weights/";
     TString prefix = "classification";
     TString method = "KNN";

    // Book method(s)    
     TString methodName = method + TString(" method");
     TString weightfile = dir + prefix + "_" + method + TString(".weights.xml");
     reader->BookMVA( methodName, weightfile );

     // Prepare a file to write histograms
     TFile *file  = new TFile( "kNN.root","RECREATE" );      
     
     // Book output histograms

     ///// MPPE exercise ....
     
     // Open file with events we want to classify
       
     std::ifstream fdata("sample_good_separation/data.txt");

     Double_t knn_output;
     
     // Event loop
     while(fdata>>var1>>var2){
       // Return the MVA output
       knn_output=reader->EvaluateMVA( "KNN method");
       // Fill histogram
       ///// MPPE exercise ....
     }

     // Write histogram
     ///// MPPE exercise ....
     
     // Close the file
     file->Close();
}

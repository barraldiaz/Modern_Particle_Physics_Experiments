// Compile with command
// g++ first_root.cpp `root-config --cflags --libs`
//
#include  <TRandom3.h>
#include<iostream>

using namespace std;

int main(){

  TRandom3 r;
  for(int i=0; i<1000; i++){
    double liczba=r.Gaus(0.,7.5);
    cout << liczba << endl;
  }
    
  return 0;
}

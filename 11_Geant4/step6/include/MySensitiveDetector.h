#ifndef MySensitivewDetector_H
#define MySensitiveDetector_H

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4TouchableHistory;

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
  MySensitiveDetector(G4String);
  ~MySensitiveDetector();
private:
  virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *); 
};
#endif

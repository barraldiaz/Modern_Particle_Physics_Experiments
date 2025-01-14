#ifndef MYDETECTORCONSTRUCTION_H
#define MYDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  MyDetectorConstruction();

  ~MyDetectorConstruction();

  virtual G4VPhysicalVolume *Construct();

};
#endif

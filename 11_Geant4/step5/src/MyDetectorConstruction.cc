#include "MyDetectorConstruction.h"

#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4MaterialPropertiesTable.hh"
//#include "MySensitiveDetector.h"

MyDetectorConstruction::MyDetectorConstruction() { }

MyDetectorConstruction::~MyDetectorConstruction() { }

G4VPhysicalVolume *MyDetectorConstruction::Construct() {

  G4NistManager *nist = G4NistManager::Instance();

/*
  G4Material *SiO2 = new G4Material("SiO2",2.201*g/cm3,2);
  SiO2->AddElement(nist->FindOrBuildElement("Si"),1);
  SiO2->AddElement(nist->FindOrBuildElement("O"),2);

  G4Material *H2O = new G4Material("H2O",1.000*g/cm3,2);
  H2O->AddElement(nist->FindOrBuildElement("H"),2);
  H2O->AddElement(nist->FindOrBuildElement("O"),1);

  G4Element *C = nist->FindOrBuildElement("C");

  G4Material *Aerogel = new G4Material("Aerogel",0.200*g/cm3,3);
  Aerogel->AddMaterial(SiO2, 62.5*perCent);
  Aerogel->AddMaterial(H2O, 37.4*perCent);
  Aerogel->AddElement(C, 0.1*perCent);
  
  G4double energy[2] = {1.239841939*eV/0.2, 1.239841939*eV/0.9};
  G4double rindexAerogel[2] = { 1.1, 1.1};
  G4double rindexWorld[2] = { 1.0, 1.0};
  G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
  mptAerogel->AddProperty("RINDEX",energy,rindexAerogel,2); 
  Aerogel->SetMaterialPropertiesTable(mptAerogel);
*/

  G4Material *Pb= nist->FindOrBuildMaterial("G4_Pb");
  G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box * solidWorld = new G4Box("solidWorld",0.5*m,0.5*m,0.5*m);
  G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat,"logicWorld");  
  G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, true);

  G4Box * solidPlate= new G4Box("solidPlate",0.4*m,0.4*m,0.05*m);
  G4LogicalVolume *logicPlate = new G4LogicalVolume(solidPlate, Pb, "logicPlate");
  G4VPhysicalVolume *physPlate= new G4PVPlacement(0, G4ThreeVector(0.,0.,0.25*m), logicPlate, "physPlate", logicWorld, false, 0, true);
  
  int ndiv=10;
  G4Box *solidDet = new G4Box("solidDet",0.5/ndiv*m, 0.5/ndiv*m, 0.01*m);
  logicDet = new G4LogicalVolume(solidDet, worldMat, "logicDetector");
  
  for (G4int i=0; i<ndiv; i++) {
    for (G4int j=0; j<ndiv; j++) {
      G4VPhysicalVolume *physDet = new G4PVPlacement(0,G4ThreeVector(-0.5*m+(i+0.5)*m/ndiv, -0.5*m+(j+0.5)*m/ndiv, 0.4*m),logicDet, "physDet",logicWorld, false, j+i*ndiv, true); 
    }
  }   
   
  return physWorld;
}

/*
void MyDetectorConstruction::ConstructSDandField() {
  MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
  logicDet->SetSensitiveDetector(sensDet);
}
*/

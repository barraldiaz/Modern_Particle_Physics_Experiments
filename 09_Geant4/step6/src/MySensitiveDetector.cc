#include "MySensitiveDetector.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name) 
{ }

MySensitiveDetector::~MySensitiveDetector()
{}


G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist){
  G4Track *track = aStep->GetTrack();
  track->SetTrackStatus(fStopAndKill);
  G4StepPoint *preStepPoint = aStep->GetPreStepPoint(); 
  G4StepPoint *postStepPoint = aStep->GetPostStepPoint(); 

  G4ThreeVector position = preStepPoint->GetPosition();
//  G4cout << "secondary position: "<<position<< G4endl;
  const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
  G4int copuNo = touchable->GetCopyNumber();
  G4VPhysicalVolume *physVol = touchable->GetVolume();  
  G4ThreeVector posDetector = physVol->GetTranslation();
  G4cout << "Det number :"<<copuNo<<" and position: "<<posDetector << G4endl;
   
  return true; 
}


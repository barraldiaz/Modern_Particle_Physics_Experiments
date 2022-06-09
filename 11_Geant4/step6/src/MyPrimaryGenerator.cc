#include "MyPrimaryGenerator.h"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"

MyPrimaryGenerator::MyPrimaryGenerator() { 
  fParticleGun = new G4ParticleGun(1);
}

MyPrimaryGenerator::~MyPrimaryGenerator() {
  delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *ev) {
  G4ParticleTable * particleTable = G4ParticleTable::GetParticleTable();
  G4String beamPart="e-";
  G4ParticleDefinition *particle=particleTable->FindParticle(beamPart);
  G4ThreeVector pos(0., 0., 0.);
  G4ThreeVector mom(0.,0.,1.);

  fParticleGun->SetParticlePosition(pos);
  fParticleGun->SetParticleMomentumDirection(mom);
  fParticleGun->SetParticleMomentum(10.*GeV);
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->GeneratePrimaryVertex(ev);
}

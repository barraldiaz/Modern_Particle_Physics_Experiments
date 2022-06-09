#ifndef MyActionInitialization_H
#define MyActionInitialization_H

#include "G4VUserActionInitialization.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public:
  MyActionInitialization();
  virtual void Build() const;
};

#endif


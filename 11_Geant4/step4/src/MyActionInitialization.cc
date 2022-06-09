#include "MyActionInitialization.h"
#include "MyPrimaryGenerator.h"


MyActionInitialization::MyActionInitialization() { }

void MyActionInitialization::Build() const
{
  MyPrimaryGenerator *generator = new MyPrimaryGenerator();
  SetUserAction(generator);
}

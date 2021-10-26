////////////////////////////////////////////////////////////////////////////
// Model wheel.cc                 SIMLIB/C++
//
// Wheel on spring with damper model (Version 1)
//
// System description (governing equation):
//   y'' = ( F - D * y' - k * y ) / M
// where:
//   M is wheel mass
//   y    height of wheel center
//   D    damping coefficient
//   k    stiffness of spring
//   F    input force
//

#include "simlib.h"

struct Wheel {                  // wheel model
  Integrator v, y;
  Wheel(Input F, double M, double D, double k):
    v( (F - D*v - k*y) / M ),   // speed
    y( v ) {}                   // height
};

// objects
Constant F(100);                // constant input force
Wheel w(F, 10, 500, 5e4);       // model

// print model state
void Sample() {
  Print("%6.3f %.4g %.4g\n", T.Value(), w.y.Value(), w.v.Value()); 
}
Sampler S(Sample, 0.001);       // periodic sampling

int main() {                    // experiment description
  SetOutput("wheel.dat");
  Print("# wheel --- model of wheel+spring+damper\n");
  Print("# Time   y   v \n");
  Init(0, 0.5);                 // set initial state and time
  SetStep(1e-3, 0.1);           // numerical method step size range
  SetAccuracy(1e-5, 0.001);     // allowed (abs.+rel.) error tolerance
  Run();                        // simulation run
  SIMLIB_statistics.Output();   // print run statistics
}


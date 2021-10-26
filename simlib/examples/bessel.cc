////////////////////////////////////////////////////////////////////////////
// Model bessel.cc               SIMLIB/C++
//
// Bessel diferential equation:
//
//   y '' + (1/t) * y ' + (1 - 1/t*t) * y = 0
//
// Initial conditions: y(0.001)=0.001, y'(0.001)=0.49999
//

#include "simlib.h"

class Bessel {          // model
  Integrator yi, y;     // state variables
 public:
  Bessel():
    yi( -(1/T)*yi+(1/(T*T)-1)*y , 0.49999),
    y( yi , 0.001)  {}
  double Y() { return y.Value(); }
  double YI() { return yi.Value(); }
};

Bessel bes;

void Sample() {                 // sampling
  Print("%-8g  %g  %g\n", T.Value(), bes.Y(), bes.YI());
}
Sampler S(Sample,0.2);          // periodic event

int main() {                    // experiment
    SetOutput("bessel.dat");
    Print("# Bessel diferential equation \n");
    Print("# Time    y     y'\n");
    SetStep(1e-6, 0.1);         // stepsize interval
    Init(0.001, 50);            // init time and simulator
    Run();                      // simulation run
    SIMLIB_statistics.Output(); // print run statistics
}


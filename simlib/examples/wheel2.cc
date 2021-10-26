////////////////////////////////////////////////////////////////////////////
// Model wheel2                SIMLIB/C++
//
// Wheel+spring+damper (version 2: multiple experiments)
// System:
//   y'' = ( F - D * y' - k * y ) / M
//

#include "simlib.h"
#include <stdlib.h>

struct Wheel {                  // model
  Parameter M, D, k;
  Integrator v, y;              // state variables
  Wheel(Input F, double _M, double _D, double _k):
    M(_M), D(_D), k(_k),        // parameters
    v( (F - D*v - k*y) / M ),   // transformed equations
    y( v ) { }
    void SetM(double _M) { M=_M; } // change parameter
    void SetD(double _D) { D=_D; }
    void Setk(double _k) { k=_k; }
    void PrintParameters() {
      Print("# mass = %g kg ", M.Value());
      Print("  damping = %g ", D.Value());
      Print("  stiffness = %g \n", k.Value());
    }
};

double _m=5, _d=500, _k=5e4;    // implicit parameter values
Constant F(100);                // constant input force

Wheel w(F, _m, _d, _k);         // model

void Sample() {                 // print system state
  Print("%6.3f %.4g %.4g\n", T.Value(), w.y.Value(), w.v.Value());
}
Sampler S(Sample, 0.001);       // periodic sampling

int main(int argc, char *argv[]) {      // experiment
  SetOutput("wheel2.dat");
  Print("# wheel2 --- multiple experiments\n");
  if(argc==4) {
     _m = atof(argv[1]);
     _d = atof(argv[2]);
     _k = atof(argv[3]);
  }
  for(double m=_m/2; m<=_m*5; m*=1.2) {
    Print("\n");                        // Gnuplot spacer
    w.SetM(m);     // set parametr M
    w.SetD(_d);    // set parametr D
    w.Setk(_k);    // set parametr k
    w.PrintParameters();
    Print("# Time  y  v \n");
    Init(0,0.3);                    // init simulator and time
    SetAccuracy(1e-6,0.001);        // set max. allowed error
    Run();                          // simulation run
    SIMLIB_statistics.Output();     // print run statistics
  }
}


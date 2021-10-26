////////////////////////////////////////////////////////////////////////////
// Model: Heating                   SIMLIB/C++
//
// Simple temperature regulation using relay
//

#include "simlib.h"

const double T1 = 10,
             T2 = 50,
             K1 = 50,
             K2 = 1;

const double XValue = 10;       // input
const double StepPrn = 0.1;     // time step for printing

class System : public aContiBlock {     // model
  Integrator y2, y1;                    // state variables
 public:
  System(Input inp, double T1, double T2, double K1, double K2) :
    y2( (K1*inp-y2)/T1 ),       // transformed equations
    y1( (K2*y2-y1)/T2 )   {}
  double Value() { return y1.Value(); } // output
};

class Model : public aContiBlock {      // complete model
  Relay r;
  System s;
 public:
  Model(Input inp, double T1, double T2, double K1, double K2) :
    r( inp - s, 0, 0, 1, 1, 0, 1 ),
    s( r, T1, T2, K1, K2 )  {}
  double Value() { return s.Value(); }  // output
  double relayValue() { return r.Value(); }
};

Model s(XValue, T1, T2, K1, K2);

void Sample() {                 // print state variables, ...
    Print("%-8.2f %g %g\n", T.Value(), s.Value(), s.relayValue());
}
Sampler S(Sample, StepPrn);

int main() {                    // experiment
  SetOutput("heating.dat");
  Print("# heating --- model of temperatre regulation\n");
  Print("# Time   temp  relay\n");
  Init(0,120);                  // time initialization
  SetAccuracy(1e-2);            // required accuracy
  SetStep(1e-6,StepPrn);        // minstep important for relay
  Run();                        // simulation
  SIMLIB_statistics.Output();   // print run statistics
}


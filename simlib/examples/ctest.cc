////////////////////////////////////////////////////////////////////////////
// Model ctest2.cc               SIMLIB/C++
//
// Filter transfer function:
//
//   F(p) = 1 / (p^3 + 3.25p^2 + 16.5p + 17.5)
//
// Step response
//

#include "simlib.h"

#define OUTPUT_FILE "ctest.dat"

const double a1 = 1.0,          // filter coefficients
             a2 = 3.25,
             a3 = 16.5,
             a4 = 17.5;

const double InpValue = 1.0;    // input: step
const double StepPrn  = 0.05;   // sampling interval

class Filter : aContiBlock {
  Integrator y2, y1, y;         // state variables
 public:
  Filter(Input inp, double a1, double a2, double a3, double a4) :
    // continuous blocks interconnection:
    y2((inp-a2*y2-a3*y1-a4*y)/a1),   // y''= int(y''')
    y1(y2),                          // y'= int(y'')
    y(y1) {}                         // y= int(y')
  double Value() { return y.Value(); }  // filter output
};

Filter F(InpValue, a1, a2, a3, a4);

void Sample() {
  Print("%6.2f  %.4g \n", T.Value(), F.Value());
}
Sampler S(Sample, StepPrn);     // periodic event

int main() {                    // experiment
  SetOutput(OUTPUT_FILE);
  Print("# CTEST -- step response of filter \n");
  Print("# Time  y\n");
  Init(0,7);
  SetAccuracy(1e-3);            // relative error requested
  Run();                        // simulation
  SIMLIB_statistics.Output();   // print run statistics
}


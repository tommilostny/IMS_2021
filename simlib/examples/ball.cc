////////////////////////////////////////////////////////////////////////////
// Model ball                   SIMLIB/C++
//
// Bouncing ball (combined simulation  model, variant 1)
// Simple: zero ball diameter, bounce approximated by state-event
//

#include "simlib.h"

const double g = 9.81;          // gravity acceleration

class Ball : ConditionDown {    // ball model description
  Integrator v,y;               // state variables
  unsigned count;               // bounce event count
  void Action()  {              // state event description
      Print("# Odraz %u:\n", ++count);
      Out();                    // print state
      v = -0.8 * v.Value();     // the energy loss
      y = 0;            // this is needed for detection when small energy!
      if(count>=20)             // after 20 bounces:
        Stop();                 //   end simulation run
  }
public:
  Ball(double initialposition) :
    ConditionDown(y),           // bounce condition: (y>=0) from TRUE to FALSE
    v(-g),                      // y' = INTG( - m * g )
    y(v, initialposition),      // y  = INTG( y' )
    count(0) {}                 // init bounce count
  void Out() {
    Print("%-9.3f  % -9.3g  % -9.3g\n",
          T.Value(), y.Value(), v.Value());
  }
};

Ball m1(1.0);                   // model of system

void Sample() { m1.Out(); }     // output the ball state periodically
Sampler S(Sample,0.01);

int main() {                    // experiment description
  SetOutput("ball.dat");
  Print("# Ball --- model of bouncing ball\n");
  Print("# Time y v \n");
  Init(0);                      // initialize experiment
  SetStep(1e-10,0.5);           // bisection needs small minstep
  SetAccuracy(1e-5,0.001);      // set numerical error tolerance
  Run();                        // run simulation, print results
  SIMLIB_statistics.Output();   // print run statistics
}


////////////////////////////////////////////////////////////////////////////
// Model: pendulum.cc
// text encoding: UTF-8
// Multiple simulation experiments, simple undamped pendulum model
// Uses C++ style of input/output
// (c) PerPet 1995, 2001, 2021
// TODO: needs improvements

#include "simlib.h"
#include <iostream>
#include <fstream>
#include <iomanip>      // std::setprecision
using namespace std;

const double g = 9.81;          // gravity acceleration
double Length = 1;              // pendulum length

// equations:   x'' + omega^2 * sin(x) = 0,  omega^2 = g/L
class Pendulum {
  Parameter L;
  Integrator x1;          // angular velocity
  Integrator x;           // angle
public:
  Pendulum(double len):
    L(len),
    x1( - g/L * Sin(x) ),
    x( x1 ) { }
  void SetLength(double l) { L=l; }
  void SetInitialValues(double a, double av) {
    x.Init(a);
    x1.Init(av);
  }
  void PrintParameters(ostream & out) {
     out << "# L = " << L.Value();
     out << ", x0 = " << setprecision(16) << x.Value();
     out << ", y0 = " << x1.Value();
     out << setprecision(8) << "\n";
  }
  void PrintValues(ostream & out) {
     out << Time << ' ' << x.Value() << ' ' << x1.Value() << "\n";
  }
};

Pendulum k(Length);

// output model state
void Sample() {
  k.PrintValues(cout);
}
Sampler S(Sample, 0.1); // set sampling interval

// experiment control
int main() {
  cout << "# Model: Pendulum \n";
  string iname = "pendulum.in";
  ifstream input(iname);               // file with parameters
  if(!input) {
    cerr << "Can not open file with parameters '"<< iname << "'\n";
    return 1;
  }
  input >> Length;
  if(!input) {  // input error
    cerr << "Can not read length\n";
    return 2;
  }
  k.SetLength(Length);
  cerr << "Pendulum model: length L = " << Length << "\n";
  while(1) {
    double angle, av, t;
    input >> angle >> av >> t;
    if(!input) {                // input error
        return 0;
    }
    cerr << "# angle_0 = " << angle
         << " rad,  \t av_0 = " << setprecision(16) << av
         << " rad.s^{-1}\n";
    k.SetInitialValues(angle,av);
    k.PrintParameters(cout);
    Init(0,t);                      // experiment time span
    SetAccuracy(1e-14);             // we need extra precision
    Run();                          // simulation run
    cout << "\n";                   // extra output separator
  }
}


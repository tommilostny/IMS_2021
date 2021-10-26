////////////////////////////////////////////////////////////////////////////
// Model: pendulum.cc
// text encoding: UTF-8
// Multiple simulation experiments, simple undamped pendulum model
// Uses C++ style of output
// (c) PerPet 1995, 2001, 2021
// TODO: needs improvements

#include "simlib.h"
#include <iostream>
#include <fstream>
#include <iomanip>              // std::setprecision

const double g = 9.81;          // gravity acceleration
const double Length = 10;       // pendulum length

// table of simulation run parameters
struct param { double a; double av; double t; }
parameters[] = {
    {  1,     0,                10 },
    {  2,     0,                12 },
    {  0,     1.98090888230,    32 },
    {  0,     1.98090888231,    35 },
    {  0,     2.2,               8 },
};

// equations:   a'' + \omega^2 * sin(a) = 0,   \omega^2 = g/L
class Pendulum {
    Parameter L;        // length
    Integrator av;      // angular velocity
    Integrator a;       // angle
    public:
    Pendulum(double length):
        L(length),
        av( - g/L * Sin(a) ),
        a( av ) { }
    void SetInitialValues(double _a, double _av) {
        a.Init(_a);
        av.Init(_av);
    }
    void PrintParameters(std::ostream & out) {
        out << "# L = " << L.Value();
        out << ", a_0 = " << std::setprecision(16) << a.Value();
        out << ", av_0 = " << av.Value();
        out << std::setprecision(8) << "\n";
    }
    void PrintValues(std::ostream & out) {
        out << Time << ' ' << a.Value() << ' ' << av.Value() << "\n";
    }
};

Pendulum k(Length);

void Sample() {
    k.PrintValues(std::cout);   // output model state
}
Sampler S(Sample, 0.1);         // set sampling interval

// experiment control
int main() {
    using namespace std;
    ofstream outfile("pendulum.dat");
    auto *coutbuf = std::cout.rdbuf();  // save original buffer
    cout.rdbuf(outfile.rdbuf());        // redirect cout

    cout << "# Model: Pendulum \n";
    cerr << "  L = " << Length << "\n";
    cerr << std::setprecision(16);
    cerr << "  a_0 [rad]\tav_0 [rad.s^{-1}]\n";
    for(const auto & p: parameters) {
        cerr << "    " << p.a
             << "\t\t" << p.av
             << "\n";
        k.SetInitialValues(p.a,p.av);
        k.PrintParameters(cout);
        Init(0,p.t);                    // experiment time span
        SetAccuracy(1e-14);             // we need extra precision
                                        // - try to change to 1e-13
        Run();                          // simulation run
        cout << "\n";                   // extra output separator
    }
    cout.rdbuf(coutbuf);                // restore buffer
}


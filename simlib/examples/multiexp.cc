////////////////////////////////////////////////////////////////////////////
// Model multiexp                  SIMLIB/C++
//
// More experiments using various parameters
//

#include "simlib.h"

const double ENDTime = 2000;    // duration of simulation run

// model objects:
Facility  Box("Box");
Histogram Table("Table",0,25,20);

class Zakaznik : public Process { // customer description
  double Arrival;
  void Behavior() {             // --- customer behavior ---
    Arrival = Time;             // time of arrival
    Seize(Box);                 // try to start service (can wait)
    Wait(10);                   // service
    Release(Box);               // end the service
    Table(Time-Arrival);        // record time of waiting and service
  }
 public:
  Zakaznik() { Activate(); }
};

class Generator : public Event { // transaction generator
  double dt;
  void Behavior() {                 // --- generator behavior ---
    new Zakaznik;                   // new customer, activation
    Activate(Time+Exponential(dt)); // arrival interval
  }
 public:
  Generator(double d) : dt(d) { Activate(); }
};

void Sample() {
   if(Time>0) Print(" %g", Table.stat.MeanValue());
}
Sampler s(Sample,500);          // periodic sampling of model state

int main() {
  SetOutput("multiexp.dat");    // output redirection
  Print("# multiexp --- multiple experiments (T=%g) \n", ENDTime);
  for(int i=1; i<=20; i++)  {
    Print("# Experiment#%d \n", i);
    Init(0,ENDTime);            // init time 0..ENDTime
    Box.Clear();                // initialize all objects here
    Table.Clear();
    double interval = i;
    new Generator(interval);    // customers generator, activate
    Print("%g ", interval);
    Run();                      // single simulation experiment
    Print(" %g\n", Table.stat.MeanValue());
    SIMLIB_statistics.Output(); // print run statistics
  }
}


////////////////////////////////////////////////////////////////////////////
// Model MODEL3                   SIMLIB/C++
//
// Queuing system model with 5 servers + single queue
// Each transaction needs 1..3 servers
//

#include "simlib.h"

Store Box("Linka",5);           // 5 free positions
Histogram Table("Total time spent in system",0,5,20);

class Customer : public Process {
  double ArrivalTime;           // start time
  int ns;                       // number of servers
  void Behavior() {             // --- customer behavior ---
    ArrivalTime = Time;         // mark time
    ns = 1 + int(3*Random());   // how much to seize 1..3
    Enter(Box,ns);              // seize ns servers
    Wait(10);                   // service time
    Leave(Box,ns);              // release ns servers
    Table(Time-ArrivalTime);    // record time duration
  }
  public: Customer() { Activate(); } // init+activate
}; // Customer

class Generator : public Event {
  void Behavior() {               // --- generator behavior ---
    new Customer;                 // create new customer
    Activate(Time+Exponential(1e3/150));  // arrival interval
  }
  public: Generator() { Activate(); }
};

int main() {
  SetOutput("model3.out");
  Print(" MODEL3 \n");
  Init(0,1000);              // init time:0..1000
  new Generator;             // create and start generator
  Run();                     // simulation run
  Box.Output();              // results
  Table.Output();
  SIMLIB_statistics.Output(); // print run statistics
}


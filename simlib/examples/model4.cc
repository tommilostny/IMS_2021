////////////////////////////////////////////////////////////////////////////
// Model MODEL4                   SIMLIB/C++
//
// Example of 2 facilities with single queue (first has priority)
//

#include "simlib.h"
#include <stdio.h>

// global objects
Facility  Box[2];
Queue InQueue("Waiting customers");
Histogram Table("Time spent in system",0,5,20);

class Customer : public Process { // customer description
  double ArrivalTime;             // attribute
  void Behavior() {               // --- customer behavior ---
    ArrivalTime = Time;           // mark time
    if (!Box[0].Busy())      Seize(Box[0]); // box0 first (priority)
    else if (!Box[1].Busy()) Seize(Box[1]); // box1 second
    else {
        Into(InQueue);  // Enter queue
        Passivate();    // sleep
	// box0 or box1 will seize automatically when released
        // Warning: changed semantics from SIMLIB version 2.17
    }
    Wait(10);                     // service
    // Warning! We do not always know which box we seized
    if (Box[0].In()==this) Release(Box[0]);
    else                 Release(Box[1]);
    Table(Time-ArrivalTime);      // service time + waiting time
  }
  public: Customer() { Activate(); } // activated by constructor
};

class Generator : public Event {  // generator of customers
  void Behavior() {               // --- generator behavior ---
    new Customer;                 // create new customer
    Activate(Time+Exponential(1e3/150));  // arrival interval
  }
  public: Generator() { Activate(); } // activated by constructor
};

int main() {
  SetOutput("model4.out");
  Print(" MODEL4 \n");
  Init(0,1000);              // experiment init., time:0..1000
  Box[0].SetName("Box[0]");
  Box[1].SetName("Box[1]");
  Box[0].SetQueue(InQueue);  // both share the same queue
  Box[1].SetQueue(InQueue);
  new Generator;             // create generator
  Run();                     // simulation run
  Box[0].Output();           // print results
  Box[1].Output();
  InQueue.Output();
  Table.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//

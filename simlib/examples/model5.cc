////////////////////////////////////////////////////////////////////////////
// Model MODEL5                   SIMLIB/C++
//
// Example of 2 facilities with single queue (random)
//

#include "simlib.h"

// global objects
Facility  Box[2];
Queue InQueue("Waiting customers");
Histogram Table("Time spent in system",0,5,20);

class Customer : public Process { // customer description
  double ArrivalTime;           // start time
  int BoxNum;                   // box to use
  void Behavior() {             // --- customer behavior ---
    ArrivalTime = Time;         // mark start time
    BoxNum = (Random()<0.5) ? 0 : 1;
    if (Box[BoxNum].Busy()) {
        Into(InQueue);          // go into queue
        Passivate();            // sleep
    }
    Seize(Box[BoxNum]);         // start service
    Wait(10);                   // service
    Release(Box[BoxNum]);       // end service
    // if queue contains a customer for released box, activate it
    for( Queue::iterator p = InQueue.begin();
         p != InQueue.end();
         ++p ) {
      Customer *z = (Customer*)(*p);
      if( z->BoxNum == BoxNum ) {
        z->Out();       // remove z from queue
        z->Activate();  // wake-up
        break;
      }
    } // for
    Table(Time-ArrivalTime);  // record total time
  }
  public: Customer() { Activate(); }
}; // Customer

class Generator : public Event {  // generator of customers
  void Behavior() {               // --- customer behavior ---
    new Customer;                 // create new customer
    Activate(Time+Exponential(1e3/150));  // arrival interval
  }
  public: Generator() { Activate(); }
};

int main() {
  SetOutput("model5.out");
  Print(" MODEL5 \n");
  Init(0,1000);                 // init experiment, time:0..1000
  Box[0].SetName("Box[0]");     // naming for reports
  Box[1].SetName("Box[1]");
  new Generator;                // create and activate generator
  Run();                        // simulation run
  // print reports:
  Box[0].Output();
  Box[1].Output();
  InQueue.Output();
  Table.Output();
  SIMLIB_statistics.Output();   // print run statistics
}

//

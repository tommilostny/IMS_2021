////////////////////////////////////////////////////////////////////////////
// Model MODEL2                   SIMLIB/C++
//
// Simple model of queueing system (M/D/1)
//

#include "simlib.h"

// Global objects
Facility  Box("Server");
Histogram Table("Table of time spent in system",0,25,20);

class Customer : public Process { // Customer description
  double ArrivalTime;             // Attribute for each customer
  void Behavior() {               // - Customer behavior description
    ArrivalTime = Time;           // Mark arrival time
    Seize(Box);                   // Try to enter Box or queue
    Wait(10);                     // In service
    Release(Box);                 // Leave Box
    Table(Time-ArrivalTime);      // Record time spent in system
  }
};

class Generator : public Event {  // Generator of customers
  void Behavior() {               // - Generator behavior description
    (new Customer)->Activate();   // Create new customer, activate at Time
    Activate(Time+Exponential(1e3/150)); // Arrival time interval
  }
};

int main() {                    // Experiment description
  //DebugON();
  SetOutput("model2.out");      // Write results to file
  Print(" MODEL2 \n");
  Init(0,1000);                 // Init simulator for time 0..1000
  (new Generator)->Activate();  // Create generator, activate at 0
  Run();                        // - Simulation run
  Box.Output();                 // Print results
  Table.Output();
  SIMLIB_statistics.Output();   // Print simulator internal statistics
}


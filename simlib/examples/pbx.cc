////////////////////////////////////////////////////////////////////////////
// Model pbx.cc             SIMLIB/C++
//
// Simple example of telephone exchange model
//
// A big factory has internal telephone network with 200 phones
// connected over telephone exchange (PBX) with 6 lines to public network.
// We expect phone calls from public network each 2 minutes
// (exponential distribution). If end point is already in use, external caller
// waits with 90% probability to connection. The call lasts on average 7
// minutes (exponential). Thera are local calls each 30 seconds (exponential)
// and duration 3 minutes (exp.). The calls from local to bublic network are
// each 10 minutes and lasts 10 minutes (both exponential).

// You should evaluate average load of telephone exchange and average waiting
// time for connection for all calls.
//
// Source: homeworks (Modelling and simulation course).
//

#include "simlib.h"

////////////////////////////////////////////////////////////////////////////
// Time constants (unit = 1 minute)
const double SIM_T = 8 * 60;            // 8 hours

// Average arrival intervals:
const double E2I_CALL_INTERVAL = 2;     // inbound calls from public network
const double INT_CALL_INTERVAL = 0.5;   // internal calls
const double I2E_CALL_INTERVAL = 10;    // outbond calls to public network

// Average call durations:
const double E2I_CALL_DURATION = 7;
const double INT_CALL_DURATION = 3;
const double I2E_CALL_DURATION = 10;

////////////////////////////////////////////////////////////////////////////
// global objects
const int N_TEL = 200;          // number of telephone devices
Store PBX("PBX",6);             // hub with 6 lines
Facility Telephone[N_TEL];      // telephone devices

Histogram Table1("Waiting to connection: internal calls",0,0.1,20);
Histogram Table2("Waiting to connection: external calls",0,0.1,20);

// random device selection (Uniform distribution)
int RandomTel() {
    return int(N_TEL*Random()); // range: 0 .. N_TEL-1
}

////////////////////////////////////////////////////////////////////////////
// Call models
//
class Call : public Process {   // base
 protected:
   double t_start;              // time of arrival
   int source, destination;     // phone numbers (-1 == external)
 public:
   Call() : t_start(0), source(-1), destination(-1) { Activate(); }
};

class E2I_Call : public Call { // inbound calls from public network
   void Behavior() {
      destination = RandomTel();        // phone number
      Enter(PBX,1);             // seize one of external lines
      t_start = Time;           // internal connection start time
      if (!Telephone[destination].Busy() || Random()<0.9) {
	 // phone is available or it is busy, but caller waits (p=90%).
	 Seize(Telephone[destination]);         // start or wait
	 Table2(Time-t_start);  // record waiting time
	 Wait(Exponential(E2I_CALL_DURATION));  // talking...
	 Release(Telephone[destination]);       // hanging
      }
      Leave(PBX,1);             // release external line
   }
 public:
   static void Create() { new E2I_Call; }
};

class INT_Call : public Call {  // internal calls
   void Behavior() {
      // random selection of call source (ignore already calling phones)
      do source=RandomTel(); while(Telephone[source].Busy());
      t_start=Time;
      Seize(Telephone[source]);         // start dialing
      // random selection of call destination (destination!=source)
      do destination=RandomTel(); while(destination==source);
      Seize(Telephone[destination]);            // start talking
      Table1(Time-t_start);             // record waiting time
      Wait(Exponential(INT_CALL_DURATION));     // talking...
      Release(Telephone[destination]);          // call end
      Release(Telephone[source]);
   }
 public:
   static void Create() { new INT_Call; }
};

class I2E_Call : public Call { // outbond calls to public network
   void Behavior() {
      // random selection of call source (ignore already calling phones)
      do source=RandomTel(); while(Telephone[source].Busy());
      Seize(Telephone[source]);         // start dialing
      Enter(PBX,1);        // seize external line
      Wait(Exponential(I2E_CALL_DURATION));  // waiting+talking
      Leave(PBX,1);        // release external line
      Release(Telephone[source]);  // end of call
   }
 public:
   static void Create() { new I2E_Call; }
};

/////////////////////////////////////////////////////////////////////////////
// Call generator
//
typedef void (*CreatePtr_t)();         // pointer to static method
class Generator : public Event {       // generator
   CreatePtr_t create;  // Parameter: pointer to method T::Create()
   double dt;           // Parameter: arrival interval
   void Behavior() {
      create();                        // create new call object
      Activate(Time+Exponential(dt));  // schedule next call
   }
 public:
   Generator(CreatePtr_t p, double _dt) : create(p), dt(_dt) {
     Activate();
   }
};

/////////////////////////////////////////////////////////////////////////////
// Experiment control
//
int main() {
   SetOutput("pbx.out");
   Print("Model of telephone exchange\n");
   Init(0,SIM_T);                // init simulation time
   // create and activate generators of 3 types of calls
   new Generator(E2I_Call::Create, E2I_CALL_INTERVAL);
   new Generator(INT_Call::Create, INT_CALL_INTERVAL);
   new Generator(I2E_Call::Create, I2E_CALL_INTERVAL);
   // simulation
   Run();
   // Print results
   PBX.Output();
   Table1.Output();
   Table2.Output();
   SIMLIB_statistics.Output(); // print run statistics
}


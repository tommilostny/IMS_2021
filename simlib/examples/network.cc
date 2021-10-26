////////////////////////////////////////////////////////////////////////////
// Model network                   SIMLIB/C++
//
// Example of computer network model
//

// Benchmarks:
//                 113 s   386/40 no coprocessor
//                  31 s   386/40 + coprocessor
//                  10 s   P90/Win95/DosPrompt
//

/////////////////////////////////////////////////////////////////////////////
// Assignment:
// ----------
// The market has computer system connected by network. Each department
// has microcomputer with connected sales desks and terminal for head of the
// department. The terminal can be used for getting information on inventory
// and cash flow using questions/answers. The microcomputer is connected to
// main computer, which accepts information on sales at given intervals.
//
// Block structure of microcomputer (MCU) in department:
// +----------------------------------------------------------------------+
// |+==========+                      Bus                                 |
// || micro-   |====================================================+     |
// || processor|   |       |       |         |         |            |     |
// |+==========+   |       |       |         |         |            |     |
// |            +=====+ +=====+ +=====+  +=======+ +=======+    +=======+ |
// |            | RAM | | DMA | | PPI |  | USART | | USART | .. | USART | |
// |            +=====+ +=====+ +=====+  +=======+ +=======+    +=======+ |
// |                       |       |          |        |            |     |
// |                   +=======+   |          |        |            |     |
// |                   | USART |   |          |        |            |     |
// |                   +=======+   |          |        |            |     |
// +-----------------------|-------|----------|--------|------------|-----+
//                         |       |          |        |            |
//    central computer <===+  +==========+ +======+ +=====+      +=====+
//                            | terminal | | sd1  | | sd2 | ...  | sdN |
//                            +==========+ +======+ +=====+      +=====+
//                                                Sales desks
//
// Check if following configuration is acceptable for single department:
//
// - number of sales desks = 3
// - time interval of cash operations for 1 piece of goods is random with
//   exponential distribution and mean value 10 seconds.
// - transfer speed cash desk-microcomputer is 1200 b/s
// - each piece needs transfer of 100 B of data
// - time between questions on terminal is exponential with mean 10 minutes
// - transfer speed  terminal-microcomputer is  1 B (byte) per 100 us
// - each question or answer needs transfer of  100 to 1000 Bytes
//   (uniform distribution)
// - time to start the work on main computer is 0 to 10 s (uniform)
// - time to prepare answer on main computer is 0.5 to 15 s (uniform)
// - interval between data transfers to main computer is 5 min (exponential)
// - transfer speed main computer-microcomputer is 19200 bits/s
// - the microcomputer memory size is 24 KiB
//
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"

// Parameters: all times are in seconds [s]

#define T_END 10.0*3.6e3        // time of simulation run
#define N_CASH 3                // number of cash desks
#define T_CASH 10.0             // avg. arrival interval for cash transactions

#define V_CASH_MCU  1200.0      // [bits/s] --- communication CashDesk--MCU
#define V_TERM_MCU 80000.0      // [bits/s] --- communication Terminal--MCU
#define V_MAIN_MCU 19200.0      // [bits/s] --- communication MainComputer--MCU
#define SZ_BLK1      100        // [B] --- data size for 1 piece of goods
#define SZ_QA_MIN    100        // [B] --- min data for Question/Answer
#define SZ_QA_MAX   1000        // [B] --- max data for Question/Answer

#define T_TERM     600.0        // time interval for terminal transactions
#define T_QMAIN_MIN  0.0        // min time for sending Q to MainComputer
#define T_QMAIN_MAX 10.0        // max ...
#define T_A_MIN      0.5        // min time for Answer preparation
#define T_A_MAX     15.0        // max ...
#define T_MAIN     180.0        // time interval for MainComputer communicatin
#define MEM_SIZE 24*1024        // [B] --- memory capacity

// global objects
Store Memory("Memory", MEM_SIZE);
Facility Processor("Processor");
Facility Bus("Bus");
Facility DMA("DMA");

Histogram TCashDesk ("Service time for cash desk",0,1,10);
Histogram TTerminal ("Service time for terminal",0,5,10);
unsigned CapacityUsed = 0;  // capacity for MainComputer communication

class CashDesk : public Process {  // class of cash desk transactions
  double Arrival;
  void Behavior() {
    Arrival = Time;      // arrival time of cash desk transaction
    Seize(Processor);    // FIXME: use service priority?
    Seize(Bus);
    Enter(Memory,SZ_BLK1);
    CapacityUsed+=SZ_BLK1;
    Wait(SZ_BLK1*8/V_CASH_MCU);  // Block transfer to MCU
    Release(Bus);
    Release(Processor);
    TCashDesk(Time - Arrival);  // record duration
  }
 public: CashDesk() { Activate(); }
};

class GenerCashDesk : public Event {  // transaction generator for cash desks
  void Behavior() {
    new CashDesk;
    Activate(Time+Exponential(T_CASH));
  }
 public: GenerCashDesk() { Activate(); }
};


class Terminal : public Process { // class of terminal transactions
  double Arrival;                 // time of transaction start
  int Otazka, Odpoved;            // data size for Q/A [B]
  void Behavior() {
    Arrival = Time;
    Seize(Processor);
    Seize(Bus);
    Otazka = int(Uniform(SZ_QA_MIN,SZ_QA_MAX));
    Enter(Memory,Otazka);
    Wait(Otazka*8/V_TERM_MCU);   // question data transfer to MCU
    Release(Bus);
    Release(Processor);
    Wait(Uniform(T_QMAIN_MIN,T_QMAIN_MAX)); // prepare question
    Seize(DMA);
    Seize(Bus,1);
    Wait(Otazka*8/V_MAIN_MCU);  // question data transfer to main computer
    Leave(Memory,Otazka);
    Release(Bus);
    Release(DMA);
    Wait(Uniform(T_A_MIN,T_A_MAX)); // answer preparation
    Odpoved = int(Uniform(SZ_QA_MIN,SZ_QA_MAX));
    Seize(DMA);
    Seize(Bus,1);
    Enter(Memory,Odpoved);
    Wait(Odpoved*8/V_MAIN_MCU); // data transfer to MCU
    Release(Bus);
    Release(DMA);
    Seize(Processor);
    Seize(Bus);
    Wait(Odpoved*8/V_TERM_MCU); // data transfer to terminal
    Leave(Memory,Odpoved);
    Release(Bus);
    Release(Processor);
    TTerminal(Time - Arrival);  // record spent time
  }
 public: Terminal() { Activate(); }
};

class GenerTerminal : public Event {  // transaction generator for terminal
  void Behavior() {
    new Terminal;
    Activate(Time+Exponential(T_TERM));
  }
 public: GenerTerminal() { Activate(); }
};

class HlPocitac : public Process {   // main computer transactions
  void Behavior() {
    Seize(DMA);
    Seize(Bus);
    Wait(CapacityUsed*8/V_MAIN_MCU);   // data transfer to main computer
    Leave(Memory,CapacityUsed);
    CapacityUsed = 0;
    Release(Bus);
    Release(DMA);
  }
 public: HlPocitac() { Activate(); }
};

class GenMainComp : public Event {      // transaction generator for main comp.
  void Behavior() {
    new HlPocitac;
    Activate(Time+Exponential(T_MAIN));
  }
 public: GenMainComp() { Activate(); }
};

int main() {
  SetOutput("network.out");
  Init(0,T_END);                // init time, ...
  for (int k=0; k<N_CASH; k++)
    new GenerCashDesk;          // create generator of transactions for cash
  new GenerTerminal;            // create generator for terminal transactions
  new GenMainComp;              // create generator for main computer tasks
  Print(" network --- model of computer network\n");
  Run();
  Processor.Output();           // print results
  Bus.Output();
  DMA.Output();
  Memory.Output();
  TCashDesk.Output();
  TTerminal.Output();
  SIMLIB_statistics.Output();   // print run statistics
}


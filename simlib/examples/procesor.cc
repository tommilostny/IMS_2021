////////////////////////////////////////////////////////////////////////////
// Model procesor                 SIMLIB/C++
//
// Simple test model
//

#include "simlib.h"

#define minutes        * (60 * 1.0e3)
#define seconds        * 1.0e3
#define miliseconds    * 1.0

// total time of simulation
const double TSIM     = 30 minutes ;

// number of objects in model
const int N_CPU       = 3;
const int N_DISK      = 2;
const int N_CHANNEL   = 1;

// (average) timing
const double ARRTM    = 9 seconds ;
const double PROCTM   = 6 seconds ;
const double T_SEARCH = 80 miliseconds ;
const double T_TR     = 30 miliseconds ;

// utility: generate integer range <min,max)
// FIXME: use for integers more common <min,max>
int Uniform(int min, int max) {
  return int( Uniform(double(min), double(max/*+1*/)) );
}

// global objects
Facility  cpu[N_CPU];
Queue     q_cpu("q_cpu");

Facility  disk[N_DISK];

Facility  channel[N_CHANNEL];
Queue     q_channel("q_channel");

Store     memory("Memory",128);

Histogram table("Time", 0, 2 seconds, 25);

class Job : public Process {  // tøída po¾adavkù
  double t0;
  double tcpu;
  double tio;
  double deltaT;
  int mem;
  int recs;
  int d;
  void Behavior() {
    t0 = Time;
    Enter(memory,mem);
    while(tcpu>0)
    {
       int i,j;
       //do {
	   for(i=0; i<N_CPU; i++) 
	      if(!cpu[i].Busy()) { 
                  Seize(cpu[i]); 
                  break; 
              }
	   if(i==N_CPU) { 
              Into(q_cpu); 
              Passivate();  // wait in queue
              //Out(); // in Release operation
           }
       //}while(i==N_CPU);
       deltaT = Exponential(tio);
       Wait(deltaT);  // in cpu
       tcpu -= deltaT;
       for(i=0; i<N_CPU; i++)
         if(cpu[i].In()==this) Release(cpu[i]);
       d = Uniform(0,N_DISK);
       Seize(disk[d]);
       Wait(Uniform(0.0,T_SEARCH));  // search
       //do {
           for(j=0; j<N_CHANNEL; j++) 
               if(!channel[j].Busy()) { Seize(channel[j]); break; } 
	   if(j==N_CHANNEL) { 
              Into(q_channel); 
              Passivate();  // wait in queue
              //Out(); 
           }
       //}while(j==N_CHANNEL);
       Wait(T_TR/10 + Uniform(0.0,T_TR));  // transfer
       for(j=0; j<N_CHANNEL; j++)
         if(channel[j].In()==this) Release(channel[j]);
       Release(disk[d]);
    }
    Leave(memory,mem);
    table(Time-t0);
  }
public:
  Job() {
    tcpu = Exponential(PROCTM);
    mem  = Uniform(20,61);
    recs = (tcpu/1000 * Uniform(2.0,10.0)) + 1;
    tio  = tcpu/recs;
    Activate();
  }
};

class Generator : public Event {
  void Behavior() {
    new Job;
    Activate(Time+Exponential(ARRTM));
  }
 public:
  Generator() { Activate(); }
};

int main() {
  //DebugON();
  SetOutput("procesor.out");
  Print(" procesor --- simple computer system model\n");
  // common queue setting:
  for(int i=0; i<N_CPU; i++)      cpu[i].SetQueue(q_cpu);
  for(int i=0; i<N_CHANNEL; i++)  channel[i].SetQueue(q_channel);
  Init(0,TSIM);
  new Generator;
  Run();
  table.Output();
  SIMLIB_statistics.Output();     // print run statistics
}


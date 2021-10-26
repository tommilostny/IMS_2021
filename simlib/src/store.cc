/////////////////////////////////////////////////////////////////////////////
//! \file  store.cc  Store implementation
//
// Copyright (c) 1991-2018 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  implementation of store class (SOL-like)
//

////////////////////////////////////////////////////////////////////////////
//  interface
//

#include "simlib.h"
#include "internal.h"

#include <cstdio>


////////////////////////////////////////////////////////////////////////////
//  implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

#define _OWNQ 0x01

#define CHECKQUEUE(qptr) do { if (!qptr) SIMLIB_error(QueueRefError); }while(0)

////////////////////////////////////////////////////////////////////////////
//  constructors
//
Store::Store() :
  _Qflag(_OWNQ),
  capacity(1L),
  used(0L),
  Q(new Queue("Q"))
{
  Dprintf(("Store::Store()"));
}

Store::Store(unsigned long _capacity) :
  _Qflag(_OWNQ),
  capacity(_capacity),
  used(0L),
  Q(new Queue("Q"))
{
  Dprintf(("Store::Store(%lu)",_capacity));
}

Store::Store(const char * name, unsigned long _capacity) :
  _Qflag(_OWNQ),
  capacity(_capacity),
  used(0L),
  Q(new Queue("Q"))
{
  Dprintf(("Store::Store(\"%s\",%lu)",name,_capacity));
  ::SetName(this,name);
}

Store::Store(unsigned long _capacity, Queue *queue) :
  _Qflag(0),
  capacity(_capacity),
  used(0L),
  Q(queue)
{
  CHECKQUEUE(queue);
  Dprintf(("Store::Store(%lu,%s)",_capacity,queue->Name().c_str()));
}

Store::Store(const char *name, unsigned long _capacity, Queue *queue) :
  _Qflag(0),
  capacity(_capacity),
  used(0L),
  Q(queue)
{
  CHECKQUEUE(queue);
  Dprintf(("Store::Store(\"%s\",%lu, Queue\"%s\")",
            name, _capacity, queue->Name().c_str()));
  ::SetName(this, name);
}


////////////////////////////////////////////////////////////////////////////
//  destructor
//
Store::~Store()
{
  Dprintf(("Store::~Store() // \"%s\" ",Name().c_str()));
  Clear();
  if (OwnQueue()) delete Q;
}

////////////////////////////////////////////////////////////////////////////
///  SetCapacity
///  - change capacity of store
void Store::SetCapacity(unsigned long newcapacity)
{
  if (capacity<newcapacity ||
      (QueueLen()==0 && used<=newcapacity)
     ) capacity = newcapacity;
  else SIMLIB_error(SetCapacityError);
}

////////////////////////////////////////////////////////////////////////////
/// SetQueue
/// - use another queue
void Store::SetQueue(Queue *queue)
{
  CHECKQUEUE(queue);
  if (OwnQueue())
  {
    if (QueueLen()>0) SIMLIB_warning(SetQueueError);
    delete Q;           // delete internal queue
    _Qflag &= ~_OWNQ;
  }
  Q = queue;
}

////////////////////////////////////////////////////////////////////////////
///  Enter
///  - allocate requested capacity
void Store::Enter(Entity *e, unsigned long rcap)
{
// TODO: remove parameter e, use Current
  Dprintf(("%s.Enter(%s,%lu)",Name().c_str(),e->Name().c_str(),rcap));

  if (e != Current)
    SIMLIB_error(EntityRefError); // current process only

  if (rcap>capacity)  SIMLIB_error(EnterCapError);
  if (Free() < rcap)    // not enough space in store
  {
    QueueIn(e,rcap);    // isert into queue
    e->Passivate();     // wait to activation from Leave()
    // REACTIVATION
    // FIXME: should be re-activated only from Leave() -- add checking?
    return;             // after activation is already allocated!
  }
  used += rcap;         // allocate capacity
  tstat(used);          // update statistics
}

////////////////////////////////////////////////////////////////////////////
/// Leave
/// - free requested capacity
void Store::Leave(unsigned long rcap)
{
  Dprintf(("%s.Leave(%lu)", Name().c_str(), rcap));
  if (used<rcap)
    SIMLIB_error(LeaveManyError);
  used -= rcap ;           // free capacity
  tstat(used);  tstat.n--; // fix: correction
  if(Q->empty())
    return;
  // satisfy entities waiting in queue (starting from begin)
  Queue::iterator pp = Q->begin();   // first item in queue
  while( pp != Q->end() && !Full() ) {
      Entity *p = ((Entity*)(*pp));  // FIXME: use dynamic_cast?
      ++pp; // step forward (next action invalidates iterator)
      if (p->_RequiredCapacity > Free())
          continue; // skip if request can't be satisfied
      p->Out();                      // remove from queue
      Dprintf(("%s.Enter(%s,%lu) from queue",
                Name().c_str(), p->Name().c_str(), p->_RequiredCapacity));
      used += p->_RequiredCapacity;  // allocate capacity
      tstat(used);                   // update statistics
      p->Activate();                 // reactivate now
      // will go to Store::Enter REACTIVATION
  } // while
}

////////////////////////////////////////////////////////////////////////////
/// QueueIn
/// - insert entity into priority queue
void Store::QueueIn(Entity *e, unsigned long c)
{
  Dprintf(("%s --> input queue of %s ",e->Name().c_str(),Name().c_str()));
  e->_RequiredCapacity = c;     // mark requested capacity
  Q->Insert(e);                 // insert
}

////////////////////////////////////////////////////////////////////////////
/// Clear
/// - store (re-)initialization, including internal queue
void Store::Clear()
{
  Dprintf(("%s.Clear()", Name().c_str()));
  used = 0;                     // empty store
  // FIXME: clear unconditionally? (what to do for shared queue?)
  if (OwnQueue()) Q->Clear();   // clear input queue if owned
  tstat.Clear();                // clear store statistics
}

////////////////////////////////////////////////////////////////////////////
/// OwnQueue
/// - check if store owns internal queue
bool Store::OwnQueue() const
{
  return (_Qflag & _OWNQ) != 0;
}

}


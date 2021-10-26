/////////////////////////////////////////////////////////////////////////////
//! \file event.cc   Discrete events base class
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

// implementation of class Event

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
///  constructor, default priority if not specified
Event::Event(Priority_t p) : Entity(p)
{
  Dprintf(("Event::Event(%u)", p));
}

////////////////////////////////////////////////////////////////////////////
///  destructor
Event::~Event()
{
  Dprintf(("Event::~Event()"));
}

////////////////////////////////////////////////////////////////////////////
/// activation method, called from simulation control algorithm
void Event::_Run() noexcept
{
  Behavior();
  if(Idle()) // not scheduled
      Terminate();
}

////////////////////////////////////////////////////////////////////////////
/// passivate event, it will be destroyed if not activated later in Behavior()
/// <br> Should not be used. It is there only for backwards compatibility.
void Event::Terminate()
{
  Dprintf(("%s.Terminate()",Name().c_str()));
  if(!Idle())          // if scheduled
      SQS::Get(this);  // remove from calendar
  if(isAllocated() && this != SIMLIB_Current)
      delete this;     // destroy entity (if not currently running Behavior)
}

// Event activation (scheduling) at time t
void Event::Activate(double t)
{
  Dprintf(("Event#%lu.Activate(%g)",_Ident,t));
  Entity::Activate(t);
}

#if 1
////////////////////////////////////////////////////////////////////////////
/// get name of event. It is generic "Event#" if not explicitly named
std::string Event::Name() const
{
    const std::string name = SimObject::Name();
    if(!name.empty())   return name; // has explicit name
    else                return SIMLIB_create_tmp_name("Event#%lu", _Ident);
}
#endif

} // namespace


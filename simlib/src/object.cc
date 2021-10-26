/////////////////////////////////////////////////////////////////////////////
//! \file object.cc     Root of SIMLIB/C++ class hierarchy
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// this module contains implementation of base class SimObject
//

#include "simlib.h"
#include "internal.h"
#include <unordered_map>          // used by name dictionary

////////////////////////////////////////////////////////////////////////////
namespace simlib3 {

SIMLIB_IMPLEMENTATION;
////////////////////////////////////////////////////////////////////////////

// static flag for IsAllocated()
static bool SimObject_allocated = false;

// NameDict singleton: dictionary for partial SimObject->name mapping
// Naming is not performance sensitive part of SIMLIB/C++
// We use this approach to save memory (64bit: sizeof(std::string)==32)
class NameDict {
    using TNameDict = std::unordered_map<SimObject*,std::string>;
    static TNameDict *dict;
  public:
    NameDict() {
        if(dict==nullptr) {     // can be created before construction
            dict = new TNameDict;
        }
    }
    // Set name of object
    // can be used before singleton construction
    // Warning: do not use in destructors!
    void Set(SimObject *o, const std::string &name) {
        if(dict==nullptr) {
            dict = new TNameDict;
        }
        (*dict)[o] = name;
    }
    std::string Get(const SimObject *o) const {
        if(dict==nullptr)
            return ""; // name dictionary not created -> empty name
        TNameDict::iterator it = dict->find(const_cast<SimObject*>(o));
        if(it == dict->end())  // not found
            return ""; // empty name
        return it->second;
    }
    void Erase(SimObject *o) {
        if(dict!=nullptr)
            dict->erase(o);
    }
    ~NameDict() {       // remove dictionary, all named objects -> ""
        delete dict;
        dict=nullptr;   // important for Get called after dict destruction
    }
};

NameDict::TNameDict *NameDict::dict = nullptr; // static member initialization
static NameDict name_dict; // SINGLETON, possible problems (empty names) if used after destruction

////////////////////////////////////////////////////////////////////////////
//! allocate memory for object
// TODO: optimize for small objects?
void *SimObject::operator new(size_t size) {
  void *ptr;
//  try {
    ptr = ::new char[size]; // global operator new
//  Dprintf(("SimObject::operator new(%u) = %p ", size, ptr));  // ### add extra debug level for this
//  }catch(...) { SIMLIB_error(MemoryError); }
//  if(!ptr) SIMLIB_error(MemoryError); // only for VERY old compilers
  SimObject_allocated = true; // update flag (checked in constructor)
  return ptr;
}

////////////////////////////////////////////////////////////////////////////
//! free memory
//
//TODO: this can create trouble if called from e.g. Behavior()
//
void SimObject::operator delete(void *ptr) {
//  Dprintf(("SimObject::operator delete(%p) ", ptr));
  SimObject *sp = static_cast<SimObject*>(ptr);
  if (sp->isAllocated()) {
      sp->_flags = 0; // clear all flags
      ::operator delete[](ptr);  // free memory
  }
}


////////////////////////////////////////////////////////////////////////////
//! constructor
//
SimObject::SimObject() :
  //_name(0),
  _flags(0)
{
//  Dprintf(("SimObject::SimObject() this=%p ", this));
  if(SimObject_allocated) {
    SimObject_allocated = false;
    _flags |= _ALLOCATED_FLAG;
  }
}

////////////////////////////////////////////////////////////////////////////
//! virtual destructor
//
SimObject::~SimObject()
{
//  Dprintf(("SimObject::~SimObject() this=%p ", this));
    if(HasName()) {
        name_dict.Erase(this);
    }
}

////////////////////////////////////////////////////////////////////////////
//! set the name of object
//
void SimObject::SetName(const std::string &name)
{
    name_dict.Set(this,name);
    _flags |= _HAS_NAME_FLAG;
}

////////////////////////////////////////////////////////////////////////////
//! get the name of object
//
std::string SimObject::Name() const
{
    // TODO: use RTTI+pointer if empty name by default
    return name_dict.Get(this);
}


////////////////////////////////////////////////////////////////////////////
//! print object's info
/// TODO: use operator <<
void SimObject::Output() const
{
  Print("SimObject: this=%p, name=%s\n", this, Name().c_str());  // default
}

} // namespace


/////////////////////////////////////////////////////////////////////////////
//! \file name.cc  object-name relation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// All SimObject instance can have name associated.
// Exported functions:
//
//  void SetName(SimObject *o, char *name) -- name of object o
//  void SetName(SimObject &o, char *name)
//  void RemoveName(SimObject *o)          -- remove name of o
//  void RemoveName(SimObject &o)
//  const char *GetName(SimObject *o)      -- get name of o
//  const char *GetName(SimObject &o)
//


////////////////////////////////////////////////////////////////////////////
//  interface
//

#include "simlib.h"
#include "internal.h"

////////////////////////////////////////////////////////////////////////////
//  implementation
//

#include <cstdarg> // ...
#include <cstdio>  // vsprintf()

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

/////////////////////////////////////////////////////////////////////////////
/// assign name to object
void SetName(SimObject & o, const std::string &name)
{
    o.SetName(name);
}

/// assign name to object
void SetName(SimObject * o, const std::string &name)
{
    o->SetName(name);
}

/// remove name
void RemoveName(SimObject & o)
{
    o.SetName(0);
}

/// remove name
void RemoveName(SimObject * o)
{
    o->SetName(0);
}

/// get name of object
std::string GetName(SimObject & o)
{
    return o.Name();
}

/// get name of object
std::string GetName(SimObject * o)
{
    return o->Name();
}

/// printf-like function to create temporary name
/// (the length of temporary names is limited)
/// <br> used only for printing
std::string SIMLIB_create_tmp_name(const char *fmt, ...)
{
    static char s[256];
    va_list va;
    va_start(va, fmt);
    vsnprintf(s, sizeof(s), fmt, va);
    va_end(va);
    return s;
}

} // namespace


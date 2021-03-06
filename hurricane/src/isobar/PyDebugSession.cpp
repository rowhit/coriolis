// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC 2014-2015, All Rights Reserved
//
// +-----------------------------------------------------------------+ 
// |                   C O R I O L I S                               |
// |    I s o b a r  -  Hurricane / Python Interface                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./PyDebugSession.cpp"                     |
// +-----------------------------------------------------------------+


#include "hurricane/isobar/PyDebugSession.h"
#include "hurricane/isobar/PyEntity.h"
#include "hurricane/isobar/PyCell.h"


namespace  Isobar {

using namespace Hurricane;

extern "C" {


#define  METHOD_HEAD(function)  GENERIC_METHOD_HEAD(DebugSession,session,function)


// +=================================================================+
// |            "PyDebugSession" Python Module Code Part             |
// +=================================================================+

#if defined(__PYTHON_MODULE__)


  static void* PyObject_AsHurricaneSymbol ( PyObject* object )
  {
    return (void*)EntityCast( object );
  }


  static void PyDebugSession_DeAlloc ( PyDebugSession* self )
  {
    trace << "PyDebugSession_DeAlloc(" << hex << self << ")" << endl;
  }
  

  static PyObject* PyDebugSession_open ( PyObject*, PyObject* args )
  {
    trace << "PyDebugSession_open()" << endl;

    HTRY
    PyObject*     pySymbol   = NULL;
    unsigned int  traceLevel = 10000;
    if (PyArg_ParseTuple( args
                        , "OI:DebugSession.open"
                        , &pySymbol
                        , &traceLevel
                        )) {
      void* symbol = PyObject_AsHurricaneSymbol( pySymbol );
      if (not symbol) {
        Py_RETURN_NONE;
      }
      DebugSession::open( symbol, traceLevel );
    } else {
      PyErr_SetString( ConstructorError, "Bad parameters given to DebugSession.open()." );
      return NULL;
    }
    HCATCH

    Py_RETURN_NONE;
  }
  

  static PyObject* PyDebugSession_close ( PyObject* )
  {
    trace << "PyDebugSession_close()" << endl;

    HTRY
    DebugSession::close ();
    HCATCH

    Py_RETURN_NONE;
  }
  

  static PyObject* PyDebugSession_addToTrace ( PyObject*, PyObject* args )
  {
    trace << "PyDebugSession_addToTrace()" << endl;

    HTRY
    PyObject* pySymbol = NULL;
    char*     netName  = NULL;
    if (PyArg_ParseTuple( args
                        , "O|s:DebugSession.addToTrace"
                        , &pySymbol
                        , &netName
                        )) {
      if (netName) {
        if (not IsPyCell(pySymbol)) Py_RETURN_NONE;
        Cell* cell = PYCELL_O( pySymbol );
        DebugSession::addToTrace( cell, netName );
      } else {
        void* symbol = PyObject_AsHurricaneSymbol( pySymbol );
        if (not symbol) {
          Py_RETURN_NONE;
        }
        DebugSession::addToTrace( symbol );
      }
    } else {
      PyErr_SetString( ConstructorError, "Bad parameters given to DebugSession.addToTrace()." );
      return NULL;
    }
    HCATCH

    Py_RETURN_NONE;
  }


  PyMethodDef PyDebugSession_Methods[] =
    { { "open"      , (PyCFunction)PyDebugSession_open      , METH_VARARGS|METH_CLASS
                    , "Opens a new Debug Session, tracing the given symbol." }
    , { "close"     , (PyCFunction)PyDebugSession_close     , METH_NOARGS|METH_CLASS
                    , "Closes an Update Session." }
    , { "addToTrace", (PyCFunction)PyDebugSession_addToTrace, METH_VARARGS|METH_CLASS
                    , "Adds a new object to be traced." }
    , {NULL, NULL, 0, NULL}  /* sentinel */
    };


  PyTypeObjectLinkPyTypeWithoutObject(DebugSession,DebugSession)


#else  // End of Python Module Code Part.


// +=================================================================+
// |            "PyDebugSession" Shared Library Code Part            |
// +=================================================================+


  PyTypeObjectDefinitions(DebugSession)


# endif  // Shared Library Code Part.

}  // extern "C".

}  // Isobar namespace.
 

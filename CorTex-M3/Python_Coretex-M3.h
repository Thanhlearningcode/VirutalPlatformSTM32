#pragma once 

#include "Python.h"

namespace CPU_ENV
{
    static PyObject* WriteRegister(PyObject* self, PyObject* args);
	static PyObject* ReadRegister(PyObject* self, PyObject* args);
	void ImportAPI(void);
}
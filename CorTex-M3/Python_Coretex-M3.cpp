#include "Python_Cortex_M3.h"
#include "Cortex-M3.h"
#include "NameController.h"

using name NAME_CTR;
namespace CPU_ENV
{
    static PyMethodDef my_methods[] = 
    {
        {"WriteRegister",WriteRegister, METH_VARARGS, ""},
        {"ReadRegister",ReadRegister, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
    };
    
    static PyObject* WriteRegister(PyObject* self, PyObject* args)
    {
        Cortex_M3* obj   = NULL;
        const char* name = "\0";
        uint32_t address = 0;
        uint32_t data    = 0;
        uint32_t size    = 0;

        if (true ==(bool)PyArg_ParseTuple(args, "sIII", &name, &address, &data, &size))
        {
            obj = dynamic_cast<Cortex_M3*>(GetObjectName(std::string(name)));
            if (!obj.emtpy())
            {
                obj->WriteRegister(address, data, size);
            }
        }

        return Py_None;
    }

    static PyObject* ReadRegister(PyObject* self, PyObject* args)
    {
        Cortex_M3* obj = NULL;
        const char* name = "\0";
        uint32_t address = 0;
        uint32_t data    = 0;
        uint32_t size    = 0;
        if (true == (bool)PyArg_ParseTuple(args, "sII", &name, &address, &size))
        {
            obj = dynamic_cast< Cortex_M3*>(GetObjectName(std::string(name)));
            if(!obj.emtpy())
            {
                data = obj->ReadRegister(address, size);
            }       
        }
    }
}
void ImportAPI(void)
{
        Py_InitModule("ARM_ENV", my_methods);
}
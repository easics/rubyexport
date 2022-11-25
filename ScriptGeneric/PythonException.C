// This file is part of rubyexport.
//
// rubyexport is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// rubyexport is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// rubyexport. If not, see <https://www.gnu.org/licenses/>.


#include <Python.h>
#include "PythonException.h"
#include <sstream>

PythonException::PythonException()
  : std::runtime_error("python exception")
{
}

void PythonException::checkPythonException() // static
{
  if (PyErr_Occurred())
    {
      PyObject *temp, *exc_typ, *exc_val, *exc_tb;
      PyObject * pName, * pModule, *pDict, *pFunc, *pArgs, *pValue;
      std::ostringstream err_str;

      PyErr_Fetch(&exc_typ,&exc_val,&exc_tb);
      PyErr_NormalizeException(&exc_typ,&exc_val,&exc_tb);

#if PY_MAJOR_VERSION == 2
      pName = PyString_FromString("traceback");
#endif
#if PY_MAJOR_VERSION == 3
      pName = PyUnicode_FromString("traceback");
#endif
      pModule = PyImport_Import(pName);
      Py_DECREF(pName);

      temp = PyObject_Str(exc_typ);
      if (temp != nullptr)
        {
#if (PY_MAJOR_VERSION == 2)
          err_str << PyString_AsString(temp);
#endif
#if PY_MAJOR_VERSION == 3
          err_str << PyUnicode_AsUTF8(temp);
#endif
          err_str << "\n";
        }
      temp = PyObject_Str(exc_val);
      if (temp != nullptr)
        {
#if PY_MAJOR_VERSION == 2          
          err_str << PyString_AsString(temp);
#endif
#if PY_MAJOR_VERSION == 3
          err_str << PyUnicode_AsUTF8(temp);
#endif
        }
      Py_DECREF(temp);
      err_str << "\n";

      if (exc_tb != nullptr && pModule != nullptr )
        {
          pDict = PyModule_GetDict(pModule);
          pFunc = PyDict_GetItemString(pDict, "format_tb");
          if (pFunc && PyCallable_Check(pFunc))
            {
              pArgs = PyTuple_New(1);
              PyTuple_SetItem(pArgs, 0, exc_tb);
              pValue = PyObject_CallObject(pFunc, pArgs);
              if (pValue != nullptr)
                {
                  int len = PyList_Size(pValue);
                  if (len > 0)
                    {
                    PyObject *t,*tt;
                    int i;
                    char *buffer;
                    for (i = 0; i < len; i++)
                      {
                        tt = PyList_GetItem(pValue,i);
                        t = Py_BuildValue("(O)",tt);
                        if (!PyArg_ParseTuple(t,"s",&buffer))
                          {
                            return;
                          }

                        err_str << buffer;
                        err_str << "\n";
                      }
                    }
                }
              Py_DECREF(pValue);
              Py_DECREF(pArgs);
            }
        }
      Py_DECREF(pModule);

      //PyErr_Restore(exc_typ, exc_val, exc_tb);
      //PyErr_Print();

      throw std::runtime_error(std::string("Python exception\n") + err_str.str());
    }
}


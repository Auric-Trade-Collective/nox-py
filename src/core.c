#include <stdio.h>
#include "noxpy.h"


void CreateNoxApi(NoxEndpointCollection* endp_coll) {
  NoxEndpoints = endp_coll;
  printf("[nox-py] Initializing...\n");

  PyImport_AppendInittab("noxpy", &PyInit_noxpy);
  Py_Initialize();

  PyObject* sys_path = PySys_GetObject("path");
  PyList_Append(sys_path, PyUnicode_FromString("."));

  PyObject* pName = PyUnicode_FromString("api");
  PyObject* pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != NULL) {
    printf("[nox-py] Loaded successfully.\n");
    PyObject* pFunc = PyObject_GetAttrString(pModule, "NoxMain");
    if (pFunc && PyCallable_Check(pFunc)) {
      PyObject_CallObject(pFunc, NULL);
      Py_DECREF(pFunc);
    } else {
      if (PyErr_Occurred()) PyErr_Print();
      printf("[nox-py] Failed to find NoxMain function.\n");
    }
  } else {
    PyErr_Print();
    printf("[nox-py] Failed to load api.py\n");
  }

  mainThreadState = PyEval_SaveThread();
}

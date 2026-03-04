#include "noxpy.h"

static PyObject* noxpy_initNox(PyObject* self, PyObject* args) {
  if (Nox != NULL) {
    Py_RETURN_NONE;
  }

  Nox = (NoxObject*)PyObject_CallObject((PyObject*)&NoxType, NULL);
  Nox->endpoints = PyDict_New();

  Py_INCREF(Nox);
  return (PyObject*)Nox;
}

static struct PyMethodDef NoxPyMethods[] = {
    {"initNox", noxpy_initNox, METH_NOARGS, "Initialize Nox and return the instance."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef noxpy_module = {
    PyModuleDef_HEAD_INIT, "noxpy", NULL, -1, NoxPyMethods
};

PyMODINIT_FUNC PyInit_noxpy(void) {
  PyObject* m;

  if (PyType_Ready(&PyHttpResponseType) < 0) return NULL;
  if (PyType_Ready(&PyHttpRequestType) < 0) return NULL;
  if (PyType_Ready(&NoxType) < 0) return NULL;

  m = PyModule_Create(&noxpy_module);
  if (m == NULL) return NULL;

  Py_INCREF(&NoxType);
  PyModule_AddObject(m, "Nox", (PyObject*)&NoxType);

  return m;
}

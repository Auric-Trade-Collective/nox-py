/*

NOTICE
This file is only here because major product structure refactoring was done.
This is being kept here for a little bit just in case.

*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

#include "noxpy.h"
#include "webapi.h"

typedef struct {
  PyObject_HEAD HttpResponse* resp;
} PyHttpResponseObject;

static PyObject* PyHttpResponse_writeText(PyHttpResponseObject* self,
                                          PyObject* args) {
  const char* text;
  Py_ssize_t len;

  if (!PyArg_ParseTuple(args, "s#", &text, &len)) {
    return NULL;
  }

  WriteText(self->resp, (char*)text, (int)len);
  Py_RETURN_NONE;
}

static PyMethodDef PyHttpResponse_methods[] = {
    {"writeText", (PyCFunction)PyHttpResponse_writeText, METH_VARARGS,
     "Write text to the client."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject PyHttpResponseType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "noxpy.HttpResponse",
    .tp_basicsize = sizeof(PyHttpResponseObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = PyHttpResponse_methods,
};

typedef struct {
  PyObject_HEAD HttpRequest* req;
} PyHttpRequestObject;

// req.endpoint
static PyObject* PyHttpRequest_get_endpoint(PyHttpRequestObject* self,
                                            void* closure) {
  return PyUnicode_FromString(self->req->endpoint);
}

// req.method
static PyObject* PyHttpRequest_get_method(PyHttpRequestObject* self,
                                          void* closure) {
  return PyUnicode_FromString(self->req->method);
}

// req.remoteAddr
static PyObject* PyHttpRequest_get_remoteAddr(PyHttpRequestObject* self,
                                              void* closure) {
  if (self->req->remoteAddr == NULL) {
    Py_RETURN_NONE;
  }
  return PyUnicode_FromString(self->req->remoteAddr);
}

static PyGetSetDef PyHttpRequest_getseters[] = {
    {"endpoint", (getter)PyHttpRequest_get_endpoint, NULL, "URL endpoint",
     NULL},
    {"method", (getter)PyHttpRequest_get_method, NULL, "HTTP Method", NULL},
    {"remoteAddr", (getter)PyHttpRequest_get_remoteAddr, NULL, "Remote IP",
     NULL},
    {NULL} /* Sentinel */
};

static PyTypeObject PyHttpRequestType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "noxpy.HttpRequest",
    .tp_basicsize = sizeof(PyHttpRequestObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = PyHttpRequest_getseters,
};

typedef struct {
  PyObject_HEAD PyObject* endpoints;
} NoxObject;

static NoxObject* Nox = NULL;
static NoxEndpointCollection* NoxEndpoints = NULL;
static PyThreadState* mainThreadState = NULL;

void PythonEndpointBridge(HttpResponse* resp, HttpRequest* req) {
  PyGILState_STATE gstate = PyGILState_Ensure();

  if (Nox == NULL || Nox->endpoints == NULL) {
    PyGILState_Release(gstate);
    return;
  }

  PyObject* endp = PyDict_GetItemString(Nox->endpoints, req->endpoint);
  if (endp != NULL) {
    PyObject* func = PyDict_GetItemString(endp, req->method);
    if (func != NULL && PyCallable_Check(func)) {
      PyHttpResponseObject* py_resp =
          PyObject_New(PyHttpResponseObject, &PyHttpResponseType);
      py_resp->resp = resp;

      PyHttpRequestObject* py_req =
          PyObject_New(PyHttpRequestObject, &PyHttpRequestType);
      py_req->req = req;

      PyObject* args = PyTuple_New(2);
      PyTuple_SetItem(args, 0, (PyObject*)py_resp);
      PyTuple_SetItem(args, 1, (PyObject*)py_req);

      PyObject* res = PyObject_CallObject(func, args);
      if (res == NULL) {
        PyErr_Print();
      } else {
        Py_DECREF(res);
      }
      Py_DECREF(args);
    }
  }

  PyGILState_Release(gstate);
}

static PyObject* noxpy_createGet(PyObject* self, PyObject* args) {
  const char* path;
  PyObject* func;  // Handler function

  // Parse into C types; "sO" means string, Object
  if (!PyArg_ParseTuple(args, "sO", &path, &func)) {
    return NULL;
  }

  if (!PyCallable_Check(func)) {
    PyErr_SetString(PyExc_TypeError, "Handler function must be callable");
    return NULL;
  }

  PyObject* endp = PyDict_GetItemString(Nox->endpoints, path);
  if (endp == NULL) {
    endp = PyDict_New();
    PyDict_SetItemString(Nox->endpoints, path, endp);
    Py_DECREF(endp);  // Dict now owns it, ts lowk hurt my head
  }

  printf("[nox-py] Registering GET endpoint: %s\n", path);
  PyDict_SetItemString(endp, "GET", func);
  CreateNoxEndpoint(NoxEndpoints, (char*)path, PythonEndpointBridge,
                    0);  // 0 for GET
  printf("[nox-py] Registered GET endpoint: %s\n", path);
  Py_RETURN_NONE;
}

static PyObject* noxpy_createPost(PyObject* self, PyObject* args) {
  const char* path;
  PyObject* func;  // Handler function

  // Parse into C types; "sO" means string, Object
  if (!PyArg_ParseTuple(args, "sO", &path, &func)) {
    return NULL;
  }

  if (!PyCallable_Check(func)) {
    PyErr_SetString(PyExc_TypeError, "Handler function must be callable");
    return NULL;
  }

  PyObject* endp = PyDict_GetItemString(Nox->endpoints, path);
  if (endp == NULL) {
    endp = PyDict_New();
    PyDict_SetItemString(Nox->endpoints, path, endp);
    Py_DECREF(endp);  // Dict now owns it, ts lowk hurt my head
  }

  printf("[nox-py] Registering POST endpoint: %s\n", path);
  PyDict_SetItemString(endp, "POST", func);
  CreateNoxEndpoint(NoxEndpoints, (char*)path, PythonEndpointBridge,
                    1);  // 1 for POST
  printf("[nox-py] Registered POST endpoint: %s\n", path);
  Py_RETURN_NONE;
}

// Class functions
static PyMethodDef Nox_methods[] = {
    {"createGet", (PyCFunction)noxpy_createGet, METH_VARARGS,
     "Create a GET endpoint."},
    {"createPost", (PyCFunction)noxpy_createPost, METH_VARARGS,
     "Create a POST endpoint."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject NoxType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "noxpy.Nox",
    .tp_basicsize = sizeof(NoxObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = Nox_methods,
    .tp_new = PyType_GenericNew,
};

static PyObject* noxpy_initNox(PyObject* self, PyObject* args) {
  if (Nox != NULL) {
    Py_RETURN_NONE;  // Already initialized
  }

  Nox = (NoxObject*)PyObject_CallObject((PyObject*)&NoxType, NULL);
  Nox->endpoints = PyDict_New();

  Py_INCREF(Nox);
  return (PyObject*)Nox;
}

// Module-level functions
static struct PyMethodDef NoxPyMethods[] = {
    {"initNox", noxpy_initNox, METH_NOARGS,
     "Initialize the Nox class and return the instance."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

// Module definition
static struct PyModuleDef noxpy_module = {PyModuleDef_HEAD_INIT, "noxpy", NULL,
                                          -1, NoxPyMethods};

// Init the module and register Nox class
PyMODINIT_FUNC PyInit_noxpy(void) {
  PyObject* m;  // Module

  if (PyType_Ready(&PyHttpResponseType) < 0) return NULL;
  if (PyType_Ready(&PyHttpRequestType) < 0) return NULL;
  if (PyType_Ready(&NoxType) < 0) return NULL;

  m = PyModule_Create(&noxpy_module);
  if (m == NULL) return NULL;

  Py_INCREF(&NoxType);
  if (PyModule_AddObject(m, "Nox", (PyObject*)&NoxType) < 0) {
    Py_DECREF(&NoxType);
    Py_DECREF(m);
    return NULL;
  }

  return m;
}

#ifdef _WIN32
__declspec(dllexport)
#endif
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
      if (PyErr_Occurred()) {
        PyErr_Print();
      }
      printf("[nox-py] Failed to find NoxMain function.\n");
    }

  } else {
    PyErr_Print();
    printf("[nox-py] Failed to load api.py\n");
  }

  mainThreadState = PyEval_SaveThread();
}
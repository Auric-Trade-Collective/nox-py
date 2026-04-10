#include <stdio.h>
#include "noxpy.h"
#include "enums.h"


// Global defs
NoxObject* Nox = NULL;
NoxEndpointCollection* NoxEndpoints = NULL;
PyThreadState* mainThreadState = NULL;

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
  PyObject* func;

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
    Py_DECREF(endp); // Dict now owns it, ts lowk hurt my head
  }

  printf("[nox-py] Registering GET endpoint: %s\n", path);
  PyDict_SetItemString(endp, "GET", func);
  CreateNoxEndpoint(NoxEndpoints, (char*)path, PythonEndpointBridge, HTTP_GET); 
  printf("[nox-py] Registered GET endpoint: %s\n", path);
  Py_RETURN_NONE;
}

static PyObject* noxpy_createPost(PyObject* self, PyObject* args) {
  const char* path;
  PyObject* func;

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
    Py_DECREF(endp); // Dict now owns it
  }

  printf("[nox-py] Registering POST endpoint: %s\n", path);
  PyDict_SetItemString(endp, "POST", func);
  CreateNoxEndpoint(NoxEndpoints, (char*)path, PythonEndpointBridge, HTTP_POST); 
  printf("[nox-py] Registered POST endpoint: %s\n", path);
  Py_RETURN_NONE;
}

static PyObject* noxpy_createPut(PyObject* self, PyObject* args) {
  const char* path;
  PyObject* func;

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
    Py_DECREF(endp); // Dict now owns it
  }

  printf("[nox-py] Registering PUT endpoint: %s\n", path);
  PyDict_SetItemString(endp, "PUT", func);
  CreateNoxEndpoint(NoxEndpoints, (char*)path, PythonEndpointBridge, HTTP_PUT); 
  printf("[nox-py] Registered PUT endpoint: %s\n", path);
  Py_RETURN_NONE;
}

static PyObject* noxpy_createDelete(PyObject* self, PyObject* args) {
  const char* path;
  PyObject* func;

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
    Py_DECREF(endp); // Dict now owns it
  }

  printf("[nox-py] Registering DELETE endpoint: %s\n", path);
  PyDict_SetItemString(endp, "DELETE", func);
  CreateNoxEndpoint(NoxEndpoints, (char*)path, PythonEndpointBridge, HTTP_DELETE); 
  printf("[nox-py] Registered DELETE endpoint: %s\n", path);
  Py_RETURN_NONE;
}


static PyMethodDef Nox_methods[] = {
    {"createGet", (PyCFunction)noxpy_createGet, METH_VARARGS, "Create a GET endpoint."},
    {"createPost", (PyCFunction)noxpy_createPost, METH_VARARGS, "Create a POST endpoint."},
    {"createPut", (PyCFunction)noxpy_createPut, METH_VARARGS, "Create a PUT endpoint."},
    {"createDelete", (PyCFunction)noxpy_createDelete, METH_VARARGS, "Create a DELETE endpoint."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyTypeObject NoxType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "noxpy.Nox",
    .tp_basicsize = sizeof(NoxObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = Nox_methods,
    .tp_new = PyType_GenericNew,
};

#ifndef NOXPY_H
#define NOXPY_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "webapi.h"


typedef struct {
  PyObject_HEAD HttpResponse* resp;
} PyHttpResponseObject;

typedef struct {
  PyObject_HEAD HttpRequest* req;
} PyHttpRequestObject;

typedef struct {
  PyObject_HEAD PyObject* endpoints;
} NoxObject;


extern NoxObject* Nox;
extern NoxEndpointCollection* NoxEndpoints;
extern PyThreadState* mainThreadState;


extern PyTypeObject PyHttpResponseType;
extern PyTypeObject PyHttpRequestType;
extern PyTypeObject NoxType;


extern PyMethodDef PyHttpResponse_methods[];


void PythonEndpointBridge(HttpResponse* resp, HttpRequest* req);


PyMODINIT_FUNC PyInit_noxpy(void);

#endif // NOXPY_H
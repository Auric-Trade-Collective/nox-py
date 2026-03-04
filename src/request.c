#include "noxpy.h"

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

PyTypeObject PyHttpRequestType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "noxpy.HttpRequest",
    .tp_basicsize = sizeof(PyHttpRequestObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = PyHttpRequest_getseters,
};

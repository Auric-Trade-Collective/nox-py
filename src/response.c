#include "noxpy.h"

extern PyMethodDef PyHttpResponse_methods[];

PyTypeObject PyHttpResponseType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "noxpy.HttpResponse",
    .tp_basicsize = sizeof(PyHttpResponseObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = PyHttpResponse_methods,
};

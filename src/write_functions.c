#include "noxpy.h"

// Method: resp.writeText("...")
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


PyMethodDef PyHttpResponse_methods[] = {
    {"writeText", (PyCFunction)PyHttpResponse_writeText, METH_VARARGS,
     "Write text to the client."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

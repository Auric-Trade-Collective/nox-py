#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "native/webapi.h"
#include <stdio.h>


typedef struct {
    PyObject_HEAD
    PyObject *endpoints;
} NoxObject;

static NoxObject *Nox = NULL;
static NoxEndpointCollection *NoxEndpoints = NULL;

void PythonEndpointBridge(HttpResponse *resp, HttpRequest *req) {
    if (Nox == NULL || Nox->endpoints == NULL) {
        return;
    }

    PyObject *endp = PyDict_GetItemString(Nox->endpoints, req->endpoint);
    if (endp != NULL) {
        PyObject *func = PyDict_GetItemString(endp, req->method);
        if (func != NULL && PyCallable_Check(func)) {
            PyObject *args = PyTuple_New(2);

            Py_INCREF(Py_None);
            Py_INCREF(Py_None);
            PyTuple_SetItem(args, 0, Py_None); // Placeholder for response
            PyTuple_SetItem(args, 1, Py_None); // Placeholder for request

            PyObject *res = PyObject_CallObject(func, args);
            if (res == NULL) {
                PyErr_Print();
            } else {
                Py_DECREF(res);
            }
            Py_DECREF(args);
        }
    }
}

static PyObject* noxpy_createGet(PyObject* self, PyObject* args) {
    const char *path;
    PyObject *func; // Handler function

    // Parse into C types; "sO" means string, Object
    if (!PyArg_ParseTuple(args, "sO", &path, &func)) {
        return NULL;
    }

    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "Handler function must be callable");
        return NULL;
    }

    PyObject *endp = PyDict_GetItemString(Nox->endpoints, path);
    if (endp == NULL) {
        endp = PyDict_New();
        PyDict_SetItemString(Nox->endpoints, path, endp);
        Py_DECREF(endp); // Dict now owns it, ts lowk hurt my head
    }

    printf("[nox-py] Registering GET endpoint: %s\n", path);
    PyDict_SetItemString(endp, "GET", func);
    CreateNoxEndpoint(NoxEndpoints, (char *)path, PythonEndpointBridge, 0); // 0 for GET
    printf("[nox-py] Registered GET endpoint: %s\n", path);
    Py_RETURN_NONE;
}

// Class functions
static PyMethodDef Nox_methods[] = {
    {"createGet", (PyCFunction)noxpy_createGet, METH_VARARGS, "Create a GET endpoint."},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

static PyTypeObject NoxType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "noxpy.Nox",
    .tp_basicsize = sizeof(NoxObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = Nox_methods,
    .tp_new = PyType_GenericNew,
};

static PyObject* noxpy_initNox(PyObject* self, PyObject* args) {
    if (Nox != NULL) {
        Py_RETURN_NONE; // Already initialized
    }

    Nox = (NoxObject *)PyObject_CallObject((PyObject *)&NoxType, NULL);
    Nox->endpoints = PyDict_New();

    Py_INCREF(Nox);
    return (PyObject *)Nox;
}


// Module-level functions
static struct PyMethodDef NoxPyMethods[] = {
    {"initNox", noxpy_initNox, METH_NOARGS, "Initialize the Nox class and return the instance."},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};


// Module definition
static struct PyModuleDef noxpy_module = {
    PyModuleDef_HEAD_INIT, "noxpy", NULL, -1, NoxPyMethods
};


// Init the module and register Nox class
PyMODINIT_FUNC PyInit_noxpy(void) {
    PyObject *m; // Module

    if (PyType_Ready(&NoxType) < 0) {
        return NULL;
    }

    m = PyModule_Create(&noxpy_module);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&NoxType);
    if (PyModule_AddObject(m, "Nox", (PyObject *)&NoxType) < 0) {
        Py_DECREF(&NoxType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}


#ifdef _WIN32
__declspec(dllexport)
#endif
void CreateNoxApi(NoxEndpointCollection *endp_coll) {
    NoxEndpoints = endp_coll;
    printf("[nox-py] Initializing...\n");

    PyImport_AppendInittab("noxpy", &PyInit_noxpy);
    Py_Initialize();

    PyObject *sys_path = PySys_GetObject("path");
    PyList_Append(sys_path, PyUnicode_FromString("."));

    PyObject *pName = PyUnicode_FromString("api");
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        printf("[nox-py] Loaded successfully.\n");

        PyObject *pFunc = PyObject_GetAttrString(pModule, "NoxMain");
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
}
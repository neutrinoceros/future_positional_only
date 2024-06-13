// author: Nicolas Tessore <n.tessore@ucl.ac.uk>
// license: MIT

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>


typedef struct {
    PyObject_HEAD
    PyObject* dict;
    int number;
    PyObject* wrapped;
    PyObject* names;
    PyObject* signature;
} WrapObject;



static void
wrap_dealloc(WrapObject* self)
{
    Py_XDECREF(self->wrapped);
    Py_XDECREF(self->names);
    Py_XDECREF(self->signature);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject*
wrap_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    WrapObject* self = (WrapObject*)type->tp_alloc(type, 0);

    if (self != NULL) {
        self->number = 0;

        self->names = PyTuple_New(0);
        if (self->names == NULL) {
            Py_DECREF(self);
            return NULL;
        }

        Py_INCREF(Py_None);
        self->wrapped = Py_None;

        Py_INCREF(Py_None);
        self->signature = Py_None;
    }

    return (PyObject*)self;
}


static int
wrap_init(WrapObject* self, PyObject* args, PyObject* kwds)
{
    static char *kwlist[] = {"wrapped", "names", "signature", NULL};
    Py_ssize_t i, n_names;
    PyObject* wrapped, *names, *signature, *tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOO:wrap", kwlist,
                                     &wrapped, &names, &signature))
        return -1;

    if (!PyTuple_Check(names)) {
        PyErr_SetString(PyExc_TypeError, "names must be a tuple");
        return -1;
    }

    n_names = PyTuple_GET_SIZE(names);

    for (i = 0; i < n_names; ++i) {
        PyObject* name = PyTuple_GET_ITEM(names, i);
        if (!PyUnicode_Check(name)) {
            PyErr_Format(PyExc_TypeError, "names[%zd] must be a string", i);
            return -1;
        }
    }

    self->number = n_names - 1;

    tmp = self->wrapped;
    Py_INCREF(wrapped);
    self->wrapped = wrapped;
    Py_XDECREF(tmp);

    tmp = self->names;
    Py_INCREF(names);
    self->names = names;
    Py_XDECREF(tmp);

    tmp = self->signature;
    Py_INCREF(signature);
    self->signature = signature;
    Py_XDECREF(tmp);

    return 0;
}


static PyMemberDef wrap_members[] = {
    {"__dict__", T_OBJECT, offsetof(WrapObject, dict), READONLY},
    {"wrapped", T_OBJECT, offsetof(WrapObject, wrapped), READONLY},
    {"names", T_OBJECT, offsetof(WrapObject, names), READONLY},
    {"signature", T_OBJECT, offsetof(WrapObject, signature), READONLY},
    {NULL}
};


static PyObject*
wrap_call(WrapObject* self, PyObject* args, PyObject* kwds) {
    PyObject *result;

    // 1
    // for name in kwds, if name in self->names: deprecated_names.append(name)

    // 2
    // generate/format message

    // 3
    // emit warning

    result = PyObject_Call(self->wrapped, args, kwds);
    return result;
}


static PyObject*
wrap_get(PyObject* self, PyObject* obj, PyObject* type) {
    if (obj == Py_None || obj == NULL) {
        Py_INCREF(self);
        return self;
    }
    return PyMethod_New(self, obj);
}


static PyTypeObject WrapType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_future_positional_only.wrap",
    .tp_doc = PyDoc_STR("wrap (C impl)"),
    .tp_basicsize = sizeof(WrapObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_dictoffset = offsetof(WrapObject, dict),
    .tp_new = wrap_new,
    .tp_init = (initproc)wrap_init,
    .tp_dealloc = (destructor)wrap_dealloc,
    .tp_members = wrap_members,
    .tp_call = (ternaryfunc)wrap_call,
    .tp_descr_get = wrap_get,
};


static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_positional_defaults",
    .m_doc = PyDoc_STR("Implementation of the positional_defaults package."),
    .m_size = -1,
};


PyMODINIT_FUNC
PyInit__future_positional_only(void) {
    PyObject* m;

    if (PyType_Ready(&WrapType) < 0)
        return NULL;

    m = PyModule_Create(&module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&WrapType);
    if (PyModule_AddObject(m, "wrap", (PyObject*)&WrapType) < 0) {
        Py_DECREF(&WrapType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

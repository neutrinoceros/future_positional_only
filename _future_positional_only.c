// author: Nicolas Tessore <n.tessore@ucl.ac.uk>
// license: MIT

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>


typedef struct {
    PyObject_HEAD
    PyObject* dict;
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
    // step 1: detect deprecated keyword arguments
    PyObject *name = NULL;
    PyObject *result = NULL;
    Py_ssize_t i = 0;
    Py_ssize_t n_names = 0;

    n_names = PyTuple_GET_SIZE(self->names);
    PyObject *deprecated_kwargs[n_names];
    int n_depr = 0;

    if (kwds != NULL) {
        int has_kw = -2;
        for (i=0 ; i < n_names ; i++) {
            name = PyTuple_GET_ITEM(self->names, i);
            has_kw = PyDict_Contains(kwds, name);
            if (has_kw) {
                deprecated_kwargs[i] = name;
                ++n_depr;
            } else {
                deprecated_kwargs[i] = NULL;
            }
        }

        if (n_depr > 0) {
            // step 2: generate/format message
            char *names_str, *s, *arguments, *respectively, *pos, *pronoun;

            // tmp init values to avoid segfaults
            names_str = "<names_str>";
            pos = "<pos>";
            if (n_depr > 1) {
                //names_str = str(deprecated_kw)
                s = "s";
                arguments = " arguments";
                respectively = ", respectively";
                //pos = [pos for pos, _ in pos2kw]
                pronoun = "them";
            } else {
                //names_str = repr(deprecated_kw[0])
                s = arguments = respectively = "";
                //pos, _ = pos2kw[0]
                pronoun = "it";
            }

            char *msg[1024];
            sprintf(
                msg,
                "Passing %s%s as keyword%s (at position%s %s%s) "
                "is deprecated and will stop working in a future release. "
                "Pass %s positionally to supress this warning.",
                names_str, arguments, s, s, pos, respectively, pronoun
            );
            // step 3: emit warning
            PyErr_WarnEx(PyExc_FutureWarning, msg, 2);
        }
    }

    // debug
    /*
    Py_ssize_t s = 0;
    if (args == NULL) {
        printf("args is NULL\n");
    } else {
        s = PyTuple_GET_SIZE(args);
        printf("len(args) = %i\n", s);
    }
    if (kwds == NULL) {
        printf("kwds is NULL\n");
    }  else {
        s = PyDict_Size(kwds);
        printf("len(kwds) = %i\n", s);
    }
    fflush(stdout);
    */

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

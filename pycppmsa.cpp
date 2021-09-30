#include "pycppmsa.h"
#include "ad3_msa.h"

extern "C"
{
static PyMethodDef python_methods[] =
        {
                {"run", py_run, METH_VARARGS, "run"},
                {NULL, NULL, 0, NULL}
        };

static struct PyModuleDef cModPyDem =
        {
                PyModuleDef_HEAD_INIT,
                "pycppmsa",
                NULL,
                -1,
                python_methods
        };
}

PyMODINIT_FUNC
PyInit_pycppmsa(void)
{
    return PyModule_Create(&cModPyDem);
}

PyObject* py_run(PyObject*, PyObject* args)
{
    long n_vertices;
    PyObject* py_weights;
    PyObject* py_output;
    PyObject* py_root_on_diag;

    if (!PyArg_ParseTuple(args, "lOOO", &n_vertices, &py_weights, &py_output, &py_root_on_diag))
        return NULL;

    bool root_on_diag = PyObject_IsTrue(py_root_on_diag);
    auto weights = (float*) PyLong_AsVoidPtr(py_weights);
    auto output = (int*) PyLong_AsVoidPtr(py_output);

    auto heads = msa(
            n_vertices,
            [&](int head, int mod)
            {
                if (root_on_diag)
                {
                    if (head == 0)
                        return weights[(mod - 1) * (n_vertices - 1) + mod - 1];
                    else
                        return weights[(head - 1) * (n_vertices - 1) + mod - 1];
                }
                else
                {
                    return weights[head * n_vertices + mod];
                }
            }
    );
    for (int i = 0 ; i < n_vertices ; ++i)
        output[i] = heads[i];

    return Py_None;
}

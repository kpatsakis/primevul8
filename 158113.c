builtin_locals_impl(PyObject *module)
/*[clinic end generated code: output=b46c94015ce11448 input=7874018d478d5c4b]*/
{
    PyObject *d;

    d = PyEval_GetLocals();
    Py_XINCREF(d);
    return d;
}
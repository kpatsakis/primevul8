builtin_globals_impl(PyObject *module)
/*[clinic end generated code: output=e5dd1527067b94d2 input=9327576f92bb48ba]*/
{
    PyObject *d;

    d = PyEval_GetGlobals();
    Py_XINCREF(d);
    return d;
}
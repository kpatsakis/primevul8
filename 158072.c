builtin_eval_impl(PyObject *module, PyObject *source, PyObject *globals,
                  PyObject *locals)
/*[clinic end generated code: output=0a0824aa70093116 input=11ee718a8640e527]*/
{
    PyObject *result, *source_copy;
    const char *str;
    PyCompilerFlags cf;

    if (locals != Py_None && !PyMapping_Check(locals)) {
        PyErr_SetString(PyExc_TypeError, "locals must be a mapping");
        return NULL;
    }
    if (globals != Py_None && !PyDict_Check(globals)) {
        PyErr_SetString(PyExc_TypeError, PyMapping_Check(globals) ?
            "globals must be a real dict; try eval(expr, {}, mapping)"
            : "globals must be a dict");
        return NULL;
    }
    if (globals == Py_None) {
        globals = PyEval_GetGlobals();
        if (locals == Py_None) {
            locals = PyEval_GetLocals();
            if (locals == NULL)
                return NULL;
        }
    }
    else if (locals == Py_None)
        locals = globals;

    if (globals == NULL || locals == NULL) {
        PyErr_SetString(PyExc_TypeError,
            "eval must be given globals and locals "
            "when called without a frame");
        return NULL;
    }

    if (_PyDict_GetItemId(globals, &PyId___builtins__) == NULL) {
        if (_PyDict_SetItemId(globals, &PyId___builtins__,
                              PyEval_GetBuiltins()) != 0)
            return NULL;
    }

    if (PyCode_Check(source)) {
        if (PyCode_GetNumFree((PyCodeObject *)source) > 0) {
            PyErr_SetString(PyExc_TypeError,
        "code object passed to eval() may not contain free variables");
            return NULL;
        }
        return PyEval_EvalCode(source, globals, locals);
    }

    cf.cf_flags = PyCF_SOURCE_IS_UTF8;
    str = source_as_string(source, "eval", "string, bytes or code", &cf, &source_copy);
    if (str == NULL)
        return NULL;

    while (*str == ' ' || *str == '\t')
        str++;

    (void)PyEval_MergeCompilerFlags(&cf);
    result = PyRun_StringFlags(str, Py_eval_input, globals, locals, &cf);
    Py_XDECREF(source_copy);
    return result;
}
builtin_exec_impl(PyObject *module, PyObject *source, PyObject *globals,
                  PyObject *locals)
/*[clinic end generated code: output=3c90efc6ab68ef5d input=01ca3e1c01692829]*/
{
    PyObject *v;

    if (globals == Py_None) {
        globals = PyEval_GetGlobals();
        if (locals == Py_None) {
            locals = PyEval_GetLocals();
            if (locals == NULL)
                return NULL;
        }
        if (!globals || !locals) {
            PyErr_SetString(PyExc_SystemError,
                            "globals and locals cannot be NULL");
            return NULL;
        }
    }
    else if (locals == Py_None)
        locals = globals;

    if (!PyDict_Check(globals)) {
        PyErr_Format(PyExc_TypeError, "exec() globals must be a dict, not %.100s",
                     globals->ob_type->tp_name);
        return NULL;
    }
    if (!PyMapping_Check(locals)) {
        PyErr_Format(PyExc_TypeError,
            "locals must be a mapping or None, not %.100s",
            locals->ob_type->tp_name);
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
                "code object passed to exec() may not "
                "contain free variables");
            return NULL;
        }
        v = PyEval_EvalCode(source, globals, locals);
    }
    else {
        PyObject *source_copy;
        const char *str;
        PyCompilerFlags cf;
        cf.cf_flags = PyCF_SOURCE_IS_UTF8;
        str = source_as_string(source, "exec",
                                       "string, bytes or code", &cf,
                                       &source_copy);
        if (str == NULL)
            return NULL;
        if (PyEval_MergeCompilerFlags(&cf))
            v = PyRun_StringFlags(str, Py_file_input, globals,
                                  locals, &cf);
        else
            v = PyRun_String(str, Py_file_input, globals, locals);
        Py_XDECREF(source_copy);
    }
    if (v == NULL)
        return NULL;
    Py_DECREF(v);
    Py_RETURN_NONE;
}
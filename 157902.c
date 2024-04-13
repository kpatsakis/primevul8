PyRun_InteractiveOneObjectEx(FILE *fp, PyObject *filename,
                             PyCompilerFlags *flags)
{
    PyObject *m, *d, *v, *w, *oenc = NULL, *mod_name;
    mod_ty mod;
    PyArena *arena;
    const char *ps1 = "", *ps2 = "", *enc = NULL;
    int errcode = 0;
    _Py_IDENTIFIER(encoding);
    _Py_IDENTIFIER(__main__);

    mod_name = _PyUnicode_FromId(&PyId___main__); /* borrowed */
    if (mod_name == NULL) {
        return -1;
    }

    if (fp == stdin) {
        /* Fetch encoding from sys.stdin if possible. */
        v = _PySys_GetObjectId(&PyId_stdin);
        if (v && v != Py_None) {
            oenc = _PyObject_GetAttrId(v, &PyId_encoding);
            if (oenc)
                enc = PyUnicode_AsUTF8(oenc);
            if (!enc)
                PyErr_Clear();
        }
    }
    v = _PySys_GetObjectId(&PyId_ps1);
    if (v != NULL) {
        v = PyObject_Str(v);
        if (v == NULL)
            PyErr_Clear();
        else if (PyUnicode_Check(v)) {
            ps1 = PyUnicode_AsUTF8(v);
            if (ps1 == NULL) {
                PyErr_Clear();
                ps1 = "";
            }
        }
    }
    w = _PySys_GetObjectId(&PyId_ps2);
    if (w != NULL) {
        w = PyObject_Str(w);
        if (w == NULL)
            PyErr_Clear();
        else if (PyUnicode_Check(w)) {
            ps2 = PyUnicode_AsUTF8(w);
            if (ps2 == NULL) {
                PyErr_Clear();
                ps2 = "";
            }
        }
    }
    arena = PyArena_New();
    if (arena == NULL) {
        Py_XDECREF(v);
        Py_XDECREF(w);
        Py_XDECREF(oenc);
        return -1;
    }
    mod = PyParser_ASTFromFileObject(fp, filename, enc,
                                     Py_single_input, ps1, ps2,
                                     flags, &errcode, arena);
    Py_XDECREF(v);
    Py_XDECREF(w);
    Py_XDECREF(oenc);
    if (mod == NULL) {
        PyArena_Free(arena);
        if (errcode == E_EOF) {
            PyErr_Clear();
            return E_EOF;
        }
        return -1;
    }
    m = PyImport_AddModuleObject(mod_name);
    if (m == NULL) {
        PyArena_Free(arena);
        return -1;
    }
    d = PyModule_GetDict(m);
    v = run_mod(mod, filename, d, d, flags, arena);
    PyArena_Free(arena);
    if (v == NULL) {
        return -1;
    }
    Py_DECREF(v);
    flush_io();
    return 0;
}
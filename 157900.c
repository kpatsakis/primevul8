run_pyc_file(FILE *fp, const char *filename, PyObject *globals,
             PyObject *locals, PyCompilerFlags *flags)
{
    PyCodeObject *co;
    PyObject *v;
    long magic;
    long PyImport_GetMagicNumber(void);

    magic = PyMarshal_ReadLongFromFile(fp);
    if (magic != PyImport_GetMagicNumber()) {
        if (!PyErr_Occurred())
            PyErr_SetString(PyExc_RuntimeError,
                       "Bad magic number in .pyc file");
        goto error;
    }
    /* Skip the rest of the header. */
    (void) PyMarshal_ReadLongFromFile(fp);
    (void) PyMarshal_ReadLongFromFile(fp);
    (void) PyMarshal_ReadLongFromFile(fp);
    if (PyErr_Occurred()) {
        goto error;
    }
    v = PyMarshal_ReadLastObjectFromFile(fp);
    if (v == NULL || !PyCode_Check(v)) {
        Py_XDECREF(v);
        PyErr_SetString(PyExc_RuntimeError,
                   "Bad code object in .pyc file");
        goto error;
    }
    fclose(fp);
    co = (PyCodeObject *)v;
    v = PyEval_EvalCode((PyObject*)co, globals, locals);
    if (v && flags)
        flags->cf_flags |= (co->co_flags & PyCF_MASK);
    Py_DECREF(co);
    return v;
error:
    fclose(fp);
    return NULL;
}
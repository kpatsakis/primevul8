builtin_print(PyObject *self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    static const char * const _keywords[] = {"sep", "end", "file", "flush", 0};
    static struct _PyArg_Parser _parser = {"|OOOO:print", _keywords, 0};
    PyObject *sep = NULL, *end = NULL, *file = NULL, *flush = NULL;
    int i, err;

    if (kwnames != NULL &&
            !_PyArg_ParseStackAndKeywords(args + nargs, 0, kwnames, &_parser,
                                          &sep, &end, &file, &flush)) {
        return NULL;
    }

    if (file == NULL || file == Py_None) {
        file = _PySys_GetObjectId(&PyId_stdout);
        if (file == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "lost sys.stdout");
            return NULL;
        }

        /* sys.stdout may be None when FILE* stdout isn't connected */
        if (file == Py_None)
            Py_RETURN_NONE;
    }

    if (sep == Py_None) {
        sep = NULL;
    }
    else if (sep && !PyUnicode_Check(sep)) {
        PyErr_Format(PyExc_TypeError,
                     "sep must be None or a string, not %.200s",
                     sep->ob_type->tp_name);
        return NULL;
    }
    if (end == Py_None) {
        end = NULL;
    }
    else if (end && !PyUnicode_Check(end)) {
        PyErr_Format(PyExc_TypeError,
                     "end must be None or a string, not %.200s",
                     end->ob_type->tp_name);
        return NULL;
    }

    for (i = 0; i < nargs; i++) {
        if (i > 0) {
            if (sep == NULL)
                err = PyFile_WriteString(" ", file);
            else
                err = PyFile_WriteObject(sep, file,
                                         Py_PRINT_RAW);
            if (err)
                return NULL;
        }
        err = PyFile_WriteObject(args[i], file, Py_PRINT_RAW);
        if (err)
            return NULL;
    }

    if (end == NULL)
        err = PyFile_WriteString("\n", file);
    else
        err = PyFile_WriteObject(end, file, Py_PRINT_RAW);
    if (err)
        return NULL;

    if (flush != NULL) {
        PyObject *tmp;
        int do_flush = PyObject_IsTrue(flush);
        if (do_flush == -1)
            return NULL;
        else if (do_flush) {
            tmp = _PyObject_CallMethodId(file, &PyId_flush, NULL);
            if (tmp == NULL)
                return NULL;
            else
                Py_DECREF(tmp);
        }
    }

    Py_RETURN_NONE;
}
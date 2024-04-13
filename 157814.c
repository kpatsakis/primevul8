builtin_input_impl(PyObject *module, PyObject *prompt)
/*[clinic end generated code: output=83db5a191e7a0d60 input=5e8bb70c2908fe3c]*/
{
    PyObject *fin = _PySys_GetObjectId(&PyId_stdin);
    PyObject *fout = _PySys_GetObjectId(&PyId_stdout);
    PyObject *ferr = _PySys_GetObjectId(&PyId_stderr);
    PyObject *tmp;
    long fd;
    int tty;

    /* Check that stdin/out/err are intact */
    if (fin == NULL || fin == Py_None) {
        PyErr_SetString(PyExc_RuntimeError,
                        "input(): lost sys.stdin");
        return NULL;
    }
    if (fout == NULL || fout == Py_None) {
        PyErr_SetString(PyExc_RuntimeError,
                        "input(): lost sys.stdout");
        return NULL;
    }
    if (ferr == NULL || ferr == Py_None) {
        PyErr_SetString(PyExc_RuntimeError,
                        "input(): lost sys.stderr");
        return NULL;
    }

    /* First of all, flush stderr */
    tmp = _PyObject_CallMethodId(ferr, &PyId_flush, NULL);
    if (tmp == NULL)
        PyErr_Clear();
    else
        Py_DECREF(tmp);

    /* We should only use (GNU) readline if Python's sys.stdin and
       sys.stdout are the same as C's stdin and stdout, because we
       need to pass it those. */
    tmp = _PyObject_CallMethodId(fin, &PyId_fileno, NULL);
    if (tmp == NULL) {
        PyErr_Clear();
        tty = 0;
    }
    else {
        fd = PyLong_AsLong(tmp);
        Py_DECREF(tmp);
        if (fd < 0 && PyErr_Occurred())
            return NULL;
        tty = fd == fileno(stdin) && isatty(fd);
    }
    if (tty) {
        tmp = _PyObject_CallMethodId(fout, &PyId_fileno, NULL);
        if (tmp == NULL) {
            PyErr_Clear();
            tty = 0;
        }
        else {
            fd = PyLong_AsLong(tmp);
            Py_DECREF(tmp);
            if (fd < 0 && PyErr_Occurred())
                return NULL;
            tty = fd == fileno(stdout) && isatty(fd);
        }
    }

    /* If we're interactive, use (GNU) readline */
    if (tty) {
        PyObject *po = NULL;
        const char *promptstr;
        char *s = NULL;
        PyObject *stdin_encoding = NULL, *stdin_errors = NULL;
        PyObject *stdout_encoding = NULL, *stdout_errors = NULL;
        const char *stdin_encoding_str, *stdin_errors_str;
        PyObject *result;
        size_t len;

        /* stdin is a text stream, so it must have an encoding. */
        stdin_encoding = _PyObject_GetAttrId(fin, &PyId_encoding);
        stdin_errors = _PyObject_GetAttrId(fin, &PyId_errors);
        if (!stdin_encoding || !stdin_errors ||
                !PyUnicode_Check(stdin_encoding) ||
                !PyUnicode_Check(stdin_errors)) {
            tty = 0;
            goto _readline_errors;
        }
        stdin_encoding_str = PyUnicode_AsUTF8(stdin_encoding);
        stdin_errors_str = PyUnicode_AsUTF8(stdin_errors);
        if (!stdin_encoding_str || !stdin_errors_str)
            goto _readline_errors;
        tmp = _PyObject_CallMethodId(fout, &PyId_flush, NULL);
        if (tmp == NULL)
            PyErr_Clear();
        else
            Py_DECREF(tmp);
        if (prompt != NULL) {
            /* We have a prompt, encode it as stdout would */
            const char *stdout_encoding_str, *stdout_errors_str;
            PyObject *stringpo;
            stdout_encoding = _PyObject_GetAttrId(fout, &PyId_encoding);
            stdout_errors = _PyObject_GetAttrId(fout, &PyId_errors);
            if (!stdout_encoding || !stdout_errors ||
                    !PyUnicode_Check(stdout_encoding) ||
                    !PyUnicode_Check(stdout_errors)) {
                tty = 0;
                goto _readline_errors;
            }
            stdout_encoding_str = PyUnicode_AsUTF8(stdout_encoding);
            stdout_errors_str = PyUnicode_AsUTF8(stdout_errors);
            if (!stdout_encoding_str || !stdout_errors_str)
                goto _readline_errors;
            stringpo = PyObject_Str(prompt);
            if (stringpo == NULL)
                goto _readline_errors;
            po = PyUnicode_AsEncodedString(stringpo,
                stdout_encoding_str, stdout_errors_str);
            Py_CLEAR(stdout_encoding);
            Py_CLEAR(stdout_errors);
            Py_CLEAR(stringpo);
            if (po == NULL)
                goto _readline_errors;
            assert(PyBytes_Check(po));
            promptstr = PyBytes_AS_STRING(po);
        }
        else {
            po = NULL;
            promptstr = "";
        }
        s = PyOS_Readline(stdin, stdout, promptstr);
        if (s == NULL) {
            PyErr_CheckSignals();
            if (!PyErr_Occurred())
                PyErr_SetNone(PyExc_KeyboardInterrupt);
            goto _readline_errors;
        }

        len = strlen(s);
        if (len == 0) {
            PyErr_SetNone(PyExc_EOFError);
            result = NULL;
        }
        else {
            if (len > PY_SSIZE_T_MAX) {
                PyErr_SetString(PyExc_OverflowError,
                                "input: input too long");
                result = NULL;
            }
            else {
                len--;   /* strip trailing '\n' */
                if (len != 0 && s[len-1] == '\r')
                    len--;   /* strip trailing '\r' */
                result = PyUnicode_Decode(s, len, stdin_encoding_str,
                                                  stdin_errors_str);
            }
        }
        Py_DECREF(stdin_encoding);
        Py_DECREF(stdin_errors);
        Py_XDECREF(po);
        PyMem_FREE(s);
        return result;

    _readline_errors:
        Py_XDECREF(stdin_encoding);
        Py_XDECREF(stdout_encoding);
        Py_XDECREF(stdin_errors);
        Py_XDECREF(stdout_errors);
        Py_XDECREF(po);
        if (tty)
            return NULL;

        PyErr_Clear();
    }

    /* Fallback if we're not interactive */
    if (prompt != NULL) {
        if (PyFile_WriteObject(prompt, fout, Py_PRINT_RAW) != 0)
            return NULL;
    }
    tmp = _PyObject_CallMethodId(fout, &PyId_flush, NULL);
    if (tmp == NULL)
        PyErr_Clear();
    else
        Py_DECREF(tmp);
    return PyFile_GetLine(fin, -1);
}
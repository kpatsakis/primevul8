source_as_string(PyObject *cmd, const char *funcname, const char *what, PyCompilerFlags *cf, PyObject **cmd_copy)
{
    const char *str;
    Py_ssize_t size;
    Py_buffer view;

    *cmd_copy = NULL;
    if (PyUnicode_Check(cmd)) {
        cf->cf_flags |= PyCF_IGNORE_COOKIE;
        str = PyUnicode_AsUTF8AndSize(cmd, &size);
        if (str == NULL)
            return NULL;
    }
    else if (PyBytes_Check(cmd)) {
        str = PyBytes_AS_STRING(cmd);
        size = PyBytes_GET_SIZE(cmd);
    }
    else if (PyByteArray_Check(cmd)) {
        str = PyByteArray_AS_STRING(cmd);
        size = PyByteArray_GET_SIZE(cmd);
    }
    else if (PyObject_GetBuffer(cmd, &view, PyBUF_SIMPLE) == 0) {
        /* Copy to NUL-terminated buffer. */
        *cmd_copy = PyBytes_FromStringAndSize(
            (const char *)view.buf, view.len);
        PyBuffer_Release(&view);
        if (*cmd_copy == NULL) {
            return NULL;
        }
        str = PyBytes_AS_STRING(*cmd_copy);
        size = PyBytes_GET_SIZE(*cmd_copy);
    }
    else {
        PyErr_Format(PyExc_TypeError,
          "%s() arg 1 must be a %s object",
          funcname, what);
        return NULL;
    }

    if (strlen(str) != (size_t)size) {
        PyErr_SetString(PyExc_ValueError,
                        "source code string cannot contain null bytes");
        Py_CLEAR(*cmd_copy);
        return NULL;
    }
    return str;
}
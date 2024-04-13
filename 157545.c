util_write_callback(int flags, char *ptr, size_t size, size_t nmemb, void *stream)
{
    CurlObject *self;
    PyObject *arglist;
    PyObject *result = NULL;
    size_t ret = 0;     /* assume error */
    PyObject *cb;
    int total_size;
    PYCURL_DECLARE_THREAD_STATE;

    /* acquire thread */
    self = (CurlObject *)stream;
    if (!PYCURL_ACQUIRE_THREAD())
        return ret;

    /* check args */
    cb = flags ? self->h_cb : self->w_cb;
    if (cb == NULL)
        goto silent_error;
    if (size <= 0 || nmemb <= 0)
        goto done;
    total_size = (int)(size * nmemb);
    if (total_size < 0 || (size_t)total_size / size != nmemb) {
        PyErr_SetString(ErrorObject, "integer overflow in write callback");
        goto verbose_error;
    }

    /* run callback */
#if PY_MAJOR_VERSION >= 3
    arglist = Py_BuildValue("(y#)", ptr, total_size);
#else
    arglist = Py_BuildValue("(s#)", ptr, total_size);
#endif
    if (arglist == NULL)
        goto verbose_error;
    result = PyEval_CallObject(cb, arglist);
    Py_DECREF(arglist);
    if (result == NULL)
        goto verbose_error;

    /* handle result */
    if (result == Py_None) {
        ret = total_size;           /* None means success */
    }
    else if (PyInt_Check(result) || PyLong_Check(result)) {
        /* if the cast to long fails, PyLong_AsLong() returns -1L */
        ret = (size_t) PyLong_AsLong(result);
    }
    else {
        PyErr_SetString(ErrorObject, "write callback must return int or None");
        goto verbose_error;
    }

done:
silent_error:
    Py_XDECREF(result);
    PYCURL_RELEASE_THREAD();
    return ret;
verbose_error:
    PyErr_Print();
    goto silent_error;
}
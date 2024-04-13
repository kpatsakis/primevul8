debug_callback(CURL *curlobj, curl_infotype type,
               char *buffer, size_t total_size, void *stream)
{
    CurlObject *self;
    PyObject *arglist;
    PyObject *result = NULL;
    int ret = 0;       /* always success */
    PYCURL_DECLARE_THREAD_STATE;

    UNUSED(curlobj);

    /* acquire thread */
    self = (CurlObject *)stream;
    if (!PYCURL_ACQUIRE_THREAD())
        return ret;

    /* check args */
    if (self->debug_cb == NULL)
        goto silent_error;
    if ((int)total_size < 0 || (size_t)((int)total_size) != total_size) {
        PyErr_SetString(ErrorObject, "integer overflow in debug callback");
        goto verbose_error;
    }

    /* run callback */
#if PY_MAJOR_VERSION >= 3
    arglist = Py_BuildValue("(iy#)", (int)type, buffer, (int)total_size);
#else
    arglist = Py_BuildValue("(is#)", (int)type, buffer, (int)total_size);
#endif
    if (arglist == NULL)
        goto verbose_error;
    result = PyEval_CallObject(self->debug_cb, arglist);
    Py_DECREF(arglist);
    if (result == NULL)
        goto verbose_error;

    /* return values from debug callbacks should be ignored */

silent_error:
    Py_XDECREF(result);
    PYCURL_RELEASE_THREAD();
    return ret;
verbose_error:
    PyErr_Print();
    goto silent_error;
}
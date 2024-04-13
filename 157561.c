ioctl_callback(CURL *curlobj, int cmd, void *stream)
{
    CurlObject *self;
    PyObject *arglist;
    PyObject *result = NULL;
    int ret = CURLIOE_FAILRESTART;       /* assume error */
    PYCURL_DECLARE_THREAD_STATE;

    UNUSED(curlobj);

    /* acquire thread */
    self = (CurlObject *)stream;
    if (!PYCURL_ACQUIRE_THREAD())
        return (curlioerr) ret;

    /* check args */
    if (self->ioctl_cb == NULL)
        goto silent_error;

    /* run callback */
    arglist = Py_BuildValue("(i)", cmd);
    if (arglist == NULL)
        goto verbose_error;
    result = PyEval_CallObject(self->ioctl_cb, arglist);
    Py_DECREF(arglist);
    if (result == NULL)
        goto verbose_error;

    /* handle result */
    if (result == Py_None) {
        ret = CURLIOE_OK;        /* None means success */
    }
    else if (PyInt_Check(result)) {
        ret = (int) PyInt_AsLong(result);
        if (ret >= CURLIOE_LAST || ret < 0) {
            PyErr_SetString(ErrorObject, "ioctl callback returned invalid value");
            goto verbose_error;
        }
    }

silent_error:
    Py_XDECREF(result);
    PYCURL_RELEASE_THREAD();
    return (curlioerr) ret;
verbose_error:
    PyErr_Print();
    goto silent_error;
}
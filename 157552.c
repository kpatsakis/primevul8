read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    CurlObject *self;
    PyObject *arglist;
    PyObject *result = NULL;

    size_t ret = CURL_READFUNC_ABORT;     /* assume error, this actually works */
    int total_size;

    PYCURL_DECLARE_THREAD_STATE;
    
    /* acquire thread */
    self = (CurlObject *)stream;
    if (!PYCURL_ACQUIRE_THREAD())
        return ret;

    /* check args */
    if (self->r_cb == NULL)
        goto silent_error;
    if (size <= 0 || nmemb <= 0)
        goto done;
    total_size = (int)(size * nmemb);
    if (total_size < 0 || (size_t)total_size / size != nmemb) {
        PyErr_SetString(ErrorObject, "integer overflow in read callback");
        goto verbose_error;
    }

    /* run callback */
    arglist = Py_BuildValue("(i)", total_size);
    if (arglist == NULL)
        goto verbose_error;
    result = PyEval_CallObject(self->r_cb, arglist);
    Py_DECREF(arglist);
    if (result == NULL)
        goto verbose_error;

    /* handle result */
    if (PyByteStr_Check(result)) {
        char *buf = NULL;
        Py_ssize_t obj_size = -1;
        Py_ssize_t r;
        r = PyByteStr_AsStringAndSize(result, &buf, &obj_size);
        if (r != 0 || obj_size < 0 || obj_size > total_size) {
            PyErr_Format(ErrorObject, "invalid return value for read callback (%ld bytes returned when at most %ld bytes were wanted)", (long)obj_size, (long)total_size);
            goto verbose_error;
        }
        memcpy(ptr, buf, obj_size);
        ret = obj_size;             /* success */
    }
    else if (PyUnicode_Check(result)) {
        char *buf = NULL;
        Py_ssize_t obj_size = -1;
        Py_ssize_t r;
        /*
        Encode with ascii codec.
        
        HTTP requires sending content-length for request body to the server
        before the request body is sent, therefore typically content length
        is given via POSTFIELDSIZE before read function is invoked to
        provide the data.
        
        However, if we encode the string using any encoding other than ascii,
        the length of encoded string may not match the length of unicode
        string we are encoding. Therefore, if client code does a simple
        len(source_string) to determine the value to supply in content-length,
        the length of bytes read may be different.
        
        To avoid this situation, we only accept ascii bytes in the string here.
        
        Encode data yourself to bytes when dealing with non-ascii data.
        */
        PyObject *encoded = PyUnicode_AsEncodedString(result, "ascii", "strict");
        if (encoded == NULL) {
            goto verbose_error;
        }
        r = PyByteStr_AsStringAndSize(encoded, &buf, &obj_size);
        if (r != 0 || obj_size < 0 || obj_size > total_size) {
            Py_DECREF(encoded);
            PyErr_Format(ErrorObject, "invalid return value for read callback (%ld bytes returned after encoding to utf-8 when at most %ld bytes were wanted)", (long)obj_size, (long)total_size);
            goto verbose_error;
        }
        memcpy(ptr, buf, obj_size);
        Py_DECREF(encoded);
        ret = obj_size;             /* success */
    }
#if PY_MAJOR_VERSION < 3
    else if (PyInt_Check(result)) {
        long r = PyInt_AsLong(result);
        if (r != CURL_READFUNC_ABORT && r != CURL_READFUNC_PAUSE)
            goto type_error;
        ret = r; /* either CURL_READFUNC_ABORT or CURL_READFUNC_PAUSE */
    }
#endif
    else if (PyLong_Check(result)) {
        long r = PyLong_AsLong(result);
        if (r != CURL_READFUNC_ABORT && r != CURL_READFUNC_PAUSE)
            goto type_error;
        ret = r; /* either CURL_READFUNC_ABORT or CURL_READFUNC_PAUSE */
    }
    else {
    type_error:
        PyErr_SetString(ErrorObject, "read callback must return a byte string or Unicode string with ASCII code points only");
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
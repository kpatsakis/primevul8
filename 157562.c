do_curl_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
    CurlObject *self;
    int res;
    int *ptr;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "", empty_keywords)) {
        return NULL;
    }

    /* Allocate python curl object */
    self = (CurlObject *) p_Curl_Type->tp_alloc(p_Curl_Type, 0);
    if (self == NULL)
        return NULL;

    /* tp_alloc is expected to return zeroed memory */
    for (ptr = (int *) &self->dict;
        ptr < (int *) (((char *) self) + sizeof(CurlObject));
        ++ptr)
            assert(*ptr == 0);

    /* Initialize curl handle */
    self->handle = curl_easy_init();
    if (self->handle == NULL)
        goto error;

    res = util_curl_init(self);
    if (res < 0)
            goto error;
    /* Success - return new object */
    return self;

error:
    Py_DECREF(self);    /* this also closes self->handle */
    PyErr_SetString(ErrorObject, "initializing curl failed");
    return NULL;
}
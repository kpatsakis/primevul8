do_curl_unsetopt(CurlObject *self, PyObject *args)
{
    int option;

    if (!PyArg_ParseTuple(args, "i:unsetopt", &option)) {
        return NULL;
    }
    if (check_curl_state(self, 1 | 2, "unsetopt") != 0) {
        return NULL;
    }

    /* early checks of option value */
    if (option <= 0)
        goto error;
    if (option >= (int)CURLOPTTYPE_OFF_T + OPTIONS_SIZE)
        goto error;
    if (option % 10000 >= OPTIONS_SIZE)
        goto error;

    return util_curl_unsetopt(self, option);

error:
    PyErr_SetString(PyExc_TypeError, "invalid arguments to unsetopt");
    return NULL;
}
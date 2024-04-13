static PyObject *do_curl_setstate(CurlObject *self, PyObject *args)
{
    PyErr_SetString(PyExc_TypeError, "Curl objects do not support deserialization");
    return NULL;
}
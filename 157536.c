static PyObject *do_curl_getstate(CurlObject *self)
{
    PyErr_SetString(PyExc_TypeError, "Curl objects do not support serialization");
    return NULL;
}
do_curl_setattro(PyObject *o, PyObject *name, PyObject *v)
{
    assert_curl_state((CurlObject *)o);
    return my_setattro(&((CurlObject *)o)->dict, name, v);
}
do_curl_setattr(CurlObject *co, char *name, PyObject *v)
{
    assert_curl_state(co);
    return my_setattr(&co->dict, name, v);
}
do_curl_getattr(CurlObject *co, char *name)
{
    assert_curl_state(co);
    return my_getattr((PyObject *)co, name, co->dict,
                      curlobject_constants, curlobject_methods);
}
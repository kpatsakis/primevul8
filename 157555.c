assert_curl_state(const CurlObject *self)
{
    assert(self != NULL);
    assert(Py_TYPE(self) == p_Curl_Type);
#ifdef WITH_THREAD
    (void) pycurl_get_thread_state(self);
#endif
}
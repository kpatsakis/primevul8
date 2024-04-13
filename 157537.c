do_curl_clear(CurlObject *self)
{
#ifdef WITH_THREAD
    assert(pycurl_get_thread_state(self) == NULL);
#endif
    util_curl_xdecref(self, PYCURL_MEMGROUP_ALL, self->handle);
    return 0;
}
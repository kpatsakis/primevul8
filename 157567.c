do_curl_close(CurlObject *self)
{
    if (check_curl_state(self, 2, "close") != 0) {
        return NULL;
    }
    util_curl_close(self);
    Py_RETURN_NONE;
}
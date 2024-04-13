do_curl_errstr(CurlObject *self)
{
    if (check_curl_state(self, 1 | 2, "errstr") != 0) {
        return NULL;
    }
    self->error[sizeof(self->error) - 1] = 0;

    return PyText_FromString(self->error);
}
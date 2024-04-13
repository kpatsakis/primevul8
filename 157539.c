do_curl_dealloc(CurlObject *self)
{
    PyObject_GC_UnTrack(self);
    Py_TRASHCAN_SAFE_BEGIN(self);

    Py_CLEAR(self->dict);
    util_curl_close(self);

    Py_TRASHCAN_SAFE_END(self);
    Curl_Type.tp_free(self);
}
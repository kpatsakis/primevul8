do_curl_pause(CurlObject *self, PyObject *args)
{
    int bitmask;
    CURLcode res;
#ifdef WITH_THREAD
    PyThreadState *saved_state;
#endif

    if (!PyArg_ParseTuple(args, "i:pause", &bitmask)) {
        return NULL;
    }
    if (check_curl_state(self, 1, "pause") != 0) {
        return NULL;
    }

#ifdef WITH_THREAD
    /* Save handle to current thread (used as context for python callbacks) */
    saved_state = self->state;
    PYCURL_BEGIN_ALLOW_THREADS

    /* We must allow threads here because unpausing a handle can cause
       some of its callbacks to be invoked immediately, from inside
       curl_easy_pause() */
#endif

    res = curl_easy_pause(self->handle, bitmask);

#ifdef WITH_THREAD
    PYCURL_END_ALLOW_THREADS

    /* Restore the thread-state to whatever it was on entry */
    self->state = saved_state;
#endif

    if (res != CURLE_OK) {
        CURLERROR_MSG("pause/unpause failed");
    } else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}
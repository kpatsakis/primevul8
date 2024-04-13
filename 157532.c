static PyObject *convert_slist(struct curl_slist *slist, int free_flags)
{
    PyObject *ret = NULL;
    struct curl_slist *slist_start = slist;

    ret = PyList_New((Py_ssize_t)0);
    if (ret == NULL) goto error;

    for ( ; slist != NULL; slist = slist->next) {
        PyObject *v = NULL;

        if (slist->data == NULL) {
            v = Py_None; Py_INCREF(v);
        } else {
            v = PyText_FromString(slist->data);
        }
        if (v == NULL || PyList_Append(ret, v) != 0) {
            Py_XDECREF(v);
            goto error;
        }
        Py_DECREF(v);
    }

    if ((free_flags & 1) && slist_start)
        curl_slist_free_all(slist_start);
    return ret;

error:
    Py_XDECREF(ret);
    if ((free_flags & 2) && slist_start)
        curl_slist_free_all(slist_start);
    return NULL;
}
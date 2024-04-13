util_curl_xdecref(CurlObject *self, int flags, CURL *handle)
{
    if (flags & PYCURL_MEMGROUP_ATTRDICT) {
        /* Decrement refcount for attributes dictionary. */
        Py_CLEAR(self->dict);
    }

    if (flags & PYCURL_MEMGROUP_MULTI) {
        /* Decrement refcount for multi_stack. */
        if (self->multi_stack != NULL) {
            CurlMultiObject *multi_stack = self->multi_stack;
            self->multi_stack = NULL;
            if (multi_stack->multi_handle != NULL && handle != NULL) {
                (void) curl_multi_remove_handle(multi_stack->multi_handle, handle);
            }
            Py_DECREF(multi_stack);
        }
    }

    if (flags & PYCURL_MEMGROUP_CALLBACK) {
        /* Decrement refcount for python callbacks. */
        Py_CLEAR(self->w_cb);
        Py_CLEAR(self->h_cb);
        Py_CLEAR(self->r_cb);
        Py_CLEAR(self->pro_cb);
        Py_CLEAR(self->debug_cb);
        Py_CLEAR(self->ioctl_cb);
        Py_CLEAR(self->opensocket_cb);
        Py_CLEAR(self->seek_cb);
    }

    if (flags & PYCURL_MEMGROUP_FILE) {
        /* Decrement refcount for python file objects. */
        Py_CLEAR(self->readdata_fp);
        Py_CLEAR(self->writedata_fp);
        Py_CLEAR(self->writeheader_fp);
    }

    if (flags & PYCURL_MEMGROUP_POSTFIELDS) {
        /* Decrement refcount for postfields object */
        Py_CLEAR(self->postfields_obj);
    }
    
    if (flags & PYCURL_MEMGROUP_SHARE) {
        /* Decrement refcount for share objects. */
        if (self->share != NULL) {
            CurlShareObject *share = self->share;
            self->share = NULL;
            if (share->share_handle != NULL && handle != NULL) {
                curl_easy_setopt(handle, CURLOPT_SHARE, NULL);
            }
            Py_DECREF(share);
        }
    }

    if (flags & PYCURL_MEMGROUP_HTTPPOST) {
        /* Decrement refcounts for httppost related references. */
        Py_CLEAR(self->httppost_ref_list);
    }
}
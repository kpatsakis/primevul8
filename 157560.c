util_curl_close(CurlObject *self)
{
    CURL *handle;

    /* Zero handle and thread-state to disallow any operations to be run
     * from now on */
    assert(self != NULL);
    assert(Py_TYPE(self) == p_Curl_Type);
    handle = self->handle;
    self->handle = NULL;
    if (handle == NULL) {
        /* Some paranoia assertions just to make sure the object
         * deallocation problem is finally really fixed... */
#ifdef WITH_THREAD
        assert(self->state == NULL);
#endif
        assert(self->multi_stack == NULL);
        assert(self->share == NULL);
        return;             /* already closed */
    }
#ifdef WITH_THREAD
    self->state = NULL;
#endif

    /* Decref multi stuff which uses this handle */
    util_curl_xdecref(self, PYCURL_MEMGROUP_MULTI, handle);
    /* Decref share which uses this handle */
    util_curl_xdecref(self, PYCURL_MEMGROUP_SHARE, handle);

    /* Cleanup curl handle - must be done without the gil */
    Py_BEGIN_ALLOW_THREADS
    curl_easy_cleanup(handle);
    Py_END_ALLOW_THREADS
    handle = NULL;

    /* Decref easy related objects */
    util_curl_xdecref(self, PYCURL_MEMGROUP_EASY, handle);

    /* Free all variables allocated by setopt */
#undef SFREE
#define SFREE(v)   if ((v) != NULL) (curl_formfree(v), (v) = NULL)
    SFREE(self->httppost);
#undef SFREE
#define SFREE(v)   if ((v) != NULL) (curl_slist_free_all(v), (v) = NULL)
    SFREE(self->httpheader);
    SFREE(self->http200aliases);
    SFREE(self->quote);
    SFREE(self->postquote);
    SFREE(self->prequote);
#ifdef HAVE_CURLOPT_RESOLVE
    SFREE(self->resolve);
#endif
#ifdef HAVE_CURL_7_20_0_OPTS
    SFREE(self->mail_rcpt);
#endif
#undef SFREE
}
opensocket_callback(void *clientp, curlsocktype purpose,
                    struct curl_sockaddr *address)
{
    PyObject *arglist;
    PyObject *result = NULL;
    PyObject *fileno_result = NULL;
    CurlObject *self;
    int ret = CURL_SOCKET_BAD;
    PYCURL_DECLARE_THREAD_STATE;

    self = (CurlObject *)clientp;
    PYCURL_ACQUIRE_THREAD();
    
    arglist = Py_BuildValue("(iiiN)", address->family, address->socktype, address->protocol, convert_protocol_address(&address->addr, address->addrlen));
    if (arglist == NULL)
        goto verbose_error;

    result = PyEval_CallObject(self->opensocket_cb, arglist);

    Py_DECREF(arglist);
    if (result == NULL) {
        goto verbose_error;
    }

    if (PyObject_HasAttrString(result, "fileno")) {
        fileno_result = PyObject_CallMethod(result, "fileno", NULL);

        if (fileno_result == NULL) {
            ret = CURL_SOCKET_BAD;
            goto verbose_error;
        }
        // normal operation:
        if (PyInt_Check(fileno_result)) {
            int sockfd = PyInt_AsLong(fileno_result);
#if defined(WIN32)
            ret = dup_winsock(sockfd, address);
#else
            ret = dup(sockfd);
#endif
            goto done;
        }
    } else {
        PyErr_SetString(ErrorObject, "Return value must be a socket.");
        ret = CURL_SOCKET_BAD;
        goto verbose_error;
    }

silent_error:
done:
    Py_XDECREF(result);
    Py_XDECREF(fileno_result);
    PYCURL_RELEASE_THREAD();
    return ret;
verbose_error:
    PyErr_Print();
    goto silent_error;
}
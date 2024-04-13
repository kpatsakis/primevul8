convert_protocol_address(struct sockaddr* saddr, unsigned int saddrlen)
{
    PyObject *res_obj = NULL;
    
    switch (saddr->sa_family)
    {
    case AF_INET:
        {
            struct sockaddr_in* sin = (struct sockaddr_in*)saddr;
            char *addr_str = (char *)PyMem_Malloc(INET_ADDRSTRLEN);
            
            if (addr_str == NULL) {
                PyErr_SetString(ErrorObject, "Out of memory");
                goto error;
            }
            
            if (inet_ntop(saddr->sa_family, &sin->sin_addr, addr_str, INET_ADDRSTRLEN) == NULL) {
                PyErr_SetFromErrno(ErrorObject);
                PyMem_Free(addr_str);
                goto error;
            }
            res_obj = Py_BuildValue("(si)", addr_str, ntohs(sin->sin_port));
            PyMem_Free(addr_str);
       }
        break;
    case AF_INET6:
        {
            struct sockaddr_in6* sin6 = (struct sockaddr_in6*)saddr;
            char *addr_str = (char *)PyMem_Malloc(INET6_ADDRSTRLEN);
            
            if (addr_str == NULL) {
                PyErr_SetString(ErrorObject, "Out of memory");
                goto error;
            }
            
            if (inet_ntop(saddr->sa_family, &sin6->sin6_addr, addr_str, INET6_ADDRSTRLEN) == NULL) {
                PyErr_SetFromErrno(ErrorObject);
                PyMem_Free(addr_str);
                goto error;
            }
            res_obj = Py_BuildValue("(si)", addr_str, ntohs(sin6->sin6_port));
            PyMem_Free(addr_str);
        }
        break;
    default:
        /* We (currently) only support IPv4/6 addresses.  Can curl even be used
           with anything else? */
        PyErr_SetString(ErrorObject, "Unsupported address family.");
    }
    
error:
    return res_obj;
}
static PyObject *getServerPrincipalDetails(PyObject *self, PyObject *args)
{
    const char *service = NULL;
    const char *hostname = NULL;
    char* result;

    if (!PyArg_ParseTuple(args, "ss", &service, &hostname))
        return NULL;

    result = server_principal_details(service, hostname);

    if (result != NULL)
    {
        PyObject* pyresult = Py_BuildValue("s", result);
        free(result);
        return pyresult;
    }
    else
        return NULL;
}
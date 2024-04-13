static PyObject *changePassword(PyObject *self, PyObject *args)
{
    const char *newpswd = NULL;
    const char *oldpswd = NULL;
    const char *user = NULL;
    int result = 0;

    if (!PyArg_ParseTuple(args, "sss", &user, &oldpswd, &newpswd))
        return NULL;

    result = change_user_krb5pwd(user, oldpswd, newpswd);

    if (result)
	return Py_INCREF(Py_True), Py_True;
    else
	return NULL;
}
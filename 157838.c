print_exception_recursive(PyObject *f, PyObject *value, PyObject *seen)
{
    int err = 0, res;
    PyObject *cause, *context;

    if (seen != NULL) {
        /* Exception chaining */
        PyObject *value_id = PyLong_FromVoidPtr(value);
        if (value_id == NULL || PySet_Add(seen, value_id) == -1)
            PyErr_Clear();
        else if (PyExceptionInstance_Check(value)) {
            PyObject *check_id = NULL;
            cause = PyException_GetCause(value);
            context = PyException_GetContext(value);
            if (cause) {
                check_id = PyLong_FromVoidPtr(cause);
                if (check_id == NULL) {
                    res = -1;
                } else {
                    res = PySet_Contains(seen, check_id);
                    Py_DECREF(check_id);
                }
                if (res == -1)
                    PyErr_Clear();
                if (res == 0) {
                    print_exception_recursive(
                        f, cause, seen);
                    err |= PyFile_WriteString(
                        cause_message, f);
                }
            }
            else if (context &&
                !((PyBaseExceptionObject *)value)->suppress_context) {
                check_id = PyLong_FromVoidPtr(context);
                if (check_id == NULL) {
                    res = -1;
                } else {
                    res = PySet_Contains(seen, check_id);
                    Py_DECREF(check_id);
                }
                if (res == -1)
                    PyErr_Clear();
                if (res == 0) {
                    print_exception_recursive(
                        f, context, seen);
                    err |= PyFile_WriteString(
                        context_message, f);
                }
            }
            Py_XDECREF(context);
            Py_XDECREF(cause);
        }
        Py_XDECREF(value_id);
    }
    print_exception(f, value);
    if (err != 0)
        PyErr_Clear();
}
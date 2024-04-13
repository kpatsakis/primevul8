PyErr_PrintEx(int set_sys_last_vars)
{
    PyObject *exception, *v, *tb, *hook;

    if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
        handle_system_exit();
    }
    PyErr_Fetch(&exception, &v, &tb);
    if (exception == NULL)
        return;
    PyErr_NormalizeException(&exception, &v, &tb);
    if (tb == NULL) {
        tb = Py_None;
        Py_INCREF(tb);
    }
    PyException_SetTraceback(v, tb);
    if (exception == NULL)
        return;
    /* Now we know v != NULL too */
    if (set_sys_last_vars) {
        if (_PySys_SetObjectId(&PyId_last_type, exception) < 0) {
            PyErr_Clear();
        }
        if (_PySys_SetObjectId(&PyId_last_value, v) < 0) {
            PyErr_Clear();
        }
        if (_PySys_SetObjectId(&PyId_last_traceback, tb) < 0) {
            PyErr_Clear();
        }
    }
    hook = _PySys_GetObjectId(&PyId_excepthook);
    if (hook) {
        PyObject* stack[3];
        PyObject *result;

        stack[0] = exception;
        stack[1] = v;
        stack[2] = tb;
        result = _PyObject_FastCall(hook, stack, 3);
        if (result == NULL) {
            PyObject *exception2, *v2, *tb2;
            if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
                handle_system_exit();
            }
            PyErr_Fetch(&exception2, &v2, &tb2);
            PyErr_NormalizeException(&exception2, &v2, &tb2);
            /* It should not be possible for exception2 or v2
               to be NULL. However PyErr_Display() can't
               tolerate NULLs, so just be safe. */
            if (exception2 == NULL) {
                exception2 = Py_None;
                Py_INCREF(exception2);
            }
            if (v2 == NULL) {
                v2 = Py_None;
                Py_INCREF(v2);
            }
            fflush(stdout);
            PySys_WriteStderr("Error in sys.excepthook:\n");
            PyErr_Display(exception2, v2, tb2);
            PySys_WriteStderr("\nOriginal exception was:\n");
            PyErr_Display(exception, v, tb);
            Py_DECREF(exception2);
            Py_DECREF(v2);
            Py_XDECREF(tb2);
        }
        Py_XDECREF(result);
    } else {
        PySys_WriteStderr("sys.excepthook is missing\n");
        PyErr_Display(exception, v, tb);
    }
    Py_XDECREF(exception);
    Py_XDECREF(v);
    Py_XDECREF(tb);
}
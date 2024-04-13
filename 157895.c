handle_system_exit(void)
{
    PyObject *exception, *value, *tb;
    int exitcode = 0;

    if (Py_InspectFlag)
        /* Don't exit if -i flag was given. This flag is set to 0
         * when entering interactive mode for inspecting. */
        return;

    PyErr_Fetch(&exception, &value, &tb);
    fflush(stdout);
    if (value == NULL || value == Py_None)
        goto done;
    if (PyExceptionInstance_Check(value)) {
        /* The error code should be in the `code' attribute. */
        _Py_IDENTIFIER(code);
        PyObject *code = _PyObject_GetAttrId(value, &PyId_code);
        if (code) {
            Py_DECREF(value);
            value = code;
            if (value == Py_None)
                goto done;
        }
        /* If we failed to dig out the 'code' attribute,
           just let the else clause below print the error. */
    }
    if (PyLong_Check(value))
        exitcode = (int)PyLong_AsLong(value);
    else {
        PyObject *sys_stderr = _PySys_GetObjectId(&PyId_stderr);
        /* We clear the exception here to avoid triggering the assertion
         * in PyObject_Str that ensures it won't silently lose exception
         * details.
         */
        PyErr_Clear();
        if (sys_stderr != NULL && sys_stderr != Py_None) {
            PyFile_WriteObject(value, sys_stderr, Py_PRINT_RAW);
        } else {
            PyObject_Print(value, stderr, Py_PRINT_RAW);
            fflush(stderr);
        }
        PySys_WriteStderr("\n");
        exitcode = 1;
    }
 done:
    /* Restore and clear the exception info, in order to properly decref
     * the exception, value, and traceback.      If we just exit instead,
     * these leak, which confuses PYTHONDUMPREFS output, and may prevent
     * some finalizers from running.
     */
    PyErr_Restore(exception, value, tb);
    PyErr_Clear();
    Py_Exit(exitcode);
    /* NOTREACHED */
}
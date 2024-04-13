PyRun_InteractiveLoopFlags(FILE *fp, const char *filename_str, PyCompilerFlags *flags)
{
    PyObject *filename, *v;
    int ret, err;
    PyCompilerFlags local_flags;
    int nomem_count = 0;
#ifdef Py_REF_DEBUG
    int show_ref_count = _PyInterpreterState_Get()->core_config.show_ref_count;
#endif

    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL) {
        PyErr_Print();
        return -1;
    }

    if (flags == NULL) {
        flags = &local_flags;
        local_flags.cf_flags = 0;
    }
    v = _PySys_GetObjectId(&PyId_ps1);
    if (v == NULL) {
        _PySys_SetObjectId(&PyId_ps1, v = PyUnicode_FromString(">>> "));
        Py_XDECREF(v);
    }
    v = _PySys_GetObjectId(&PyId_ps2);
    if (v == NULL) {
        _PySys_SetObjectId(&PyId_ps2, v = PyUnicode_FromString("... "));
        Py_XDECREF(v);
    }
    err = 0;
    do {
        ret = PyRun_InteractiveOneObjectEx(fp, filename, flags);
        if (ret == -1 && PyErr_Occurred()) {
            /* Prevent an endless loop after multiple consecutive MemoryErrors
             * while still allowing an interactive command to fail with a
             * MemoryError. */
            if (PyErr_ExceptionMatches(PyExc_MemoryError)) {
                if (++nomem_count > 16) {
                    PyErr_Clear();
                    err = -1;
                    break;
                }
            } else {
                nomem_count = 0;
            }
            PyErr_Print();
            flush_io();
        } else {
            nomem_count = 0;
        }
#ifdef Py_REF_DEBUG
        if (show_ref_count) {
            _PyDebug_PrintTotalRefs();
        }
#endif
    } while (ret != E_EOF);
    Py_DECREF(filename);
    return err;
}
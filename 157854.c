map_next(mapobject *lz)
{
    PyObject *small_stack[_PY_FASTCALL_SMALL_STACK];
    PyObject **stack;
    Py_ssize_t niters, nargs, i;
    PyObject *result = NULL;

    niters = PyTuple_GET_SIZE(lz->iters);
    if (niters <= (Py_ssize_t)Py_ARRAY_LENGTH(small_stack)) {
        stack = small_stack;
    }
    else {
        stack = PyMem_Malloc(niters * sizeof(stack[0]));
        if (stack == NULL) {
            PyErr_NoMemory();
            return NULL;
        }
    }

    nargs = 0;
    for (i=0; i < niters; i++) {
        PyObject *it = PyTuple_GET_ITEM(lz->iters, i);
        PyObject *val = Py_TYPE(it)->tp_iternext(it);
        if (val == NULL) {
            goto exit;
        }
        stack[i] = val;
        nargs++;
    }

    result = _PyObject_FastCall(lz->func, stack, nargs);

exit:
    for (i=0; i < nargs; i++) {
        Py_DECREF(stack[i]);
    }
    if (stack != small_stack) {
        PyMem_Free(stack);
    }
    return result;
}
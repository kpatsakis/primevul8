parser_richcompare(PyObject *left, PyObject *right, int op)
{
    int result;

    /* neither argument should be NULL, unless something's gone wrong */
    if (left == NULL || right == NULL) {
        PyErr_BadInternalCall();
        return NULL;
    }

    /* both arguments should be instances of PyST_Object */
    if (!PyST_Object_Check(left) || !PyST_Object_Check(right)) {
        Py_RETURN_NOTIMPLEMENTED;
    }

    if (left == right)
        /* if arguments are identical, they're equal */
        result = 0;
    else
        result = parser_compare_nodes(((PyST_Object *)left)->st_node,
                                      ((PyST_Object *)right)->st_node);

    Py_RETURN_RICHCOMPARE(result, 0, op);
}
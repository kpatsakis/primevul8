build_node_tree(PyObject *tuple)
{
    node* res = 0;
    PyObject *temp = PySequence_GetItem(tuple, 0);
    long num = -1;

    if (temp != NULL)
        num = PyLong_AsLong(temp);
    Py_XDECREF(temp);
    if (ISTERMINAL(num)) {
        /*
         *  The tuple is simple, but it doesn't start with a start symbol.
         *  Raise an exception now and be done with it.
         */
        tuple = Py_BuildValue("Os", tuple,
                    "Illegal syntax-tree; cannot start with terminal symbol.");
        PyErr_SetObject(parser_error, tuple);
        Py_XDECREF(tuple);
    }
    else if (ISNONTERMINAL(num)) {
        /*
         *  Not efficient, but that can be handled later.
         */
        int line_num = 0;
        PyObject *encoding = NULL;

        if (num == encoding_decl) {
            encoding = PySequence_GetItem(tuple, 2);
            if (encoding == NULL) {
                PyErr_SetString(parser_error, "missed encoding");
                return NULL;
            }
            if (!PyUnicode_Check(encoding)) {
                PyErr_Format(parser_error,
                             "encoding must be a string, found %.200s",
                             Py_TYPE(encoding)->tp_name);
                Py_DECREF(encoding);
                return NULL;
            }
            /* tuple isn't borrowed anymore here, need to DECREF */
            tuple = PySequence_GetSlice(tuple, 0, 2);
            if (tuple == NULL) {
                Py_DECREF(encoding);
                return NULL;
            }
        }
        res = PyNode_New(num);
        if (res != NULL) {
            if (res != build_node_children(tuple, res, &line_num)) {
                PyNode_Free(res);
                res = NULL;
            }
            if (res && encoding) {
                Py_ssize_t len;
                const char *temp;
                temp = PyUnicode_AsUTF8AndSize(encoding, &len);
                if (temp == NULL) {
                    PyNode_Free(res);
                    Py_DECREF(encoding);
                    Py_DECREF(tuple);
                    return NULL;
                }
                res->n_str = (char *)PyObject_MALLOC(len + 1);
                if (res->n_str == NULL) {
                    PyNode_Free(res);
                    Py_DECREF(encoding);
                    Py_DECREF(tuple);
                    PyErr_NoMemory();
                    return NULL;
                }
                (void) memcpy(res->n_str, temp, len + 1);
            }
        }
        if (encoding != NULL) {
            Py_DECREF(encoding);
            Py_DECREF(tuple);
        }
    }
    else {
        /*  The tuple is illegal -- if the number is neither TERMINAL nor
         *  NONTERMINAL, we can't use it.  Not sure the implementation
         *  allows this condition, but the API doesn't preclude it.
         */
        PyObject *err = Py_BuildValue("Os", tuple,
                                      "Illegal component tuple.");
        PyErr_SetObject(parser_error, err);
        Py_XDECREF(err);
    }

    return (res);
}
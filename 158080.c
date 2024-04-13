build_node_children(PyObject *tuple, node *root, int *line_num)
{
    Py_ssize_t len = PyObject_Size(tuple);
    Py_ssize_t i;
    int  err;

    if (len < 0) {
        return NULL;
    }
    for (i = 1; i < len; ++i) {
        /* elem must always be a sequence, however simple */
        PyObject* elem = PySequence_GetItem(tuple, i);
        int ok = elem != NULL;
        int type = 0;
        char *strn = 0;

        if (ok)
            ok = PySequence_Check(elem);
        if (ok) {
            PyObject *temp = PySequence_GetItem(elem, 0);
            if (temp == NULL)
                ok = 0;
            else {
                ok = PyLong_Check(temp);
                if (ok) {
                    type = _PyLong_AsInt(temp);
                    if (type == -1 && PyErr_Occurred()) {
                        Py_DECREF(temp);
                        Py_DECREF(elem);
                        return NULL;
                    }
                }
                Py_DECREF(temp);
            }
        }
        if (!ok) {
            PyObject *err = Py_BuildValue("Os", elem,
                                          "Illegal node construct.");
            PyErr_SetObject(parser_error, err);
            Py_XDECREF(err);
            Py_XDECREF(elem);
            return NULL;
        }
        if (ISTERMINAL(type)) {
            Py_ssize_t len = PyObject_Size(elem);
            PyObject *temp;
            const char *temp_str;

            if ((len != 2) && (len != 3)) {
                err_string("terminal nodes must have 2 or 3 entries");
                Py_DECREF(elem);
                return NULL;
            }
            temp = PySequence_GetItem(elem, 1);
            if (temp == NULL) {
                Py_DECREF(elem);
                return NULL;
            }
            if (!PyUnicode_Check(temp)) {
                PyErr_Format(parser_error,
                             "second item in terminal node must be a string,"
                             " found %s",
                             Py_TYPE(temp)->tp_name);
                Py_DECREF(temp);
                Py_DECREF(elem);
                return NULL;
            }
            if (len == 3) {
                PyObject *o = PySequence_GetItem(elem, 2);
                if (o == NULL) {
                    Py_DECREF(temp);
                    Py_DECREF(elem);
                    return NULL;
                }
                if (PyLong_Check(o)) {
                    int num = _PyLong_AsInt(o);
                    if (num == -1 && PyErr_Occurred()) {
                        Py_DECREF(o);
                        Py_DECREF(temp);
                        Py_DECREF(elem);
                        return NULL;
                    }
                    *line_num = num;
                }
                else {
                    PyErr_Format(parser_error,
                                 "third item in terminal node must be an"
                                 " integer, found %s",
                                 Py_TYPE(temp)->tp_name);
                    Py_DECREF(o);
                    Py_DECREF(temp);
                    Py_DECREF(elem);
                    return NULL;
                }
                Py_DECREF(o);
            }
            temp_str = PyUnicode_AsUTF8AndSize(temp, &len);
            if (temp_str == NULL) {
                Py_DECREF(temp);
                Py_DECREF(elem);
                return NULL;
            }
            strn = (char *)PyObject_MALLOC(len + 1);
            if (strn == NULL) {
                Py_DECREF(temp);
                Py_DECREF(elem);
                PyErr_NoMemory();
                return NULL;
            }
            (void) memcpy(strn, temp_str, len + 1);
            Py_DECREF(temp);
        }
        else if (!ISNONTERMINAL(type)) {
            /*
             *  It has to be one or the other; this is an error.
             *  Raise an exception.
             */
            PyObject *err = Py_BuildValue("Os", elem, "unknown node type.");
            PyErr_SetObject(parser_error, err);
            Py_XDECREF(err);
            Py_DECREF(elem);
            return NULL;
        }
        err = PyNode_AddChild(root, type, strn, *line_num, 0, *line_num, 0);
        if (err == E_NOMEM) {
            Py_DECREF(elem);
            PyObject_FREE(strn);
            PyErr_NoMemory();
            return NULL;
        }
        if (err == E_OVERFLOW) {
            Py_DECREF(elem);
            PyObject_FREE(strn);
            PyErr_SetString(PyExc_ValueError,
                            "unsupported number of child nodes");
            return NULL;
        }

        if (ISNONTERMINAL(type)) {
            node* new_child = CHILD(root, i - 1);

            if (new_child != build_node_children(elem, new_child, line_num)) {
                Py_DECREF(elem);
                return NULL;
            }
        }
        else if (type == NEWLINE) {     /* It's true:  we increment the     */
            ++(*line_num);              /* line number *after* the newline! */
        }
        Py_DECREF(elem);
    }
    return root;
}
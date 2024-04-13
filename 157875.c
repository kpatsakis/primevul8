parse_syntax_error(PyObject *err, PyObject **message, PyObject **filename,
                   int *lineno, int *offset, PyObject **text)
{
    int hold;
    PyObject *v;
    _Py_IDENTIFIER(msg);
    _Py_IDENTIFIER(filename);
    _Py_IDENTIFIER(lineno);
    _Py_IDENTIFIER(offset);
    _Py_IDENTIFIER(text);

    *message = NULL;
    *filename = NULL;

    /* new style errors.  `err' is an instance */
    *message = _PyObject_GetAttrId(err, &PyId_msg);
    if (!*message)
        goto finally;

    v = _PyObject_GetAttrId(err, &PyId_filename);
    if (!v)
        goto finally;
    if (v == Py_None) {
        Py_DECREF(v);
        *filename = _PyUnicode_FromId(&PyId_string);
        if (*filename == NULL)
            goto finally;
        Py_INCREF(*filename);
    }
    else {
        *filename = v;
    }

    v = _PyObject_GetAttrId(err, &PyId_lineno);
    if (!v)
        goto finally;
    hold = _PyLong_AsInt(v);
    Py_DECREF(v);
    if (hold < 0 && PyErr_Occurred())
        goto finally;
    *lineno = hold;

    v = _PyObject_GetAttrId(err, &PyId_offset);
    if (!v)
        goto finally;
    if (v == Py_None) {
        *offset = -1;
        Py_DECREF(v);
    } else {
        hold = _PyLong_AsInt(v);
        Py_DECREF(v);
        if (hold < 0 && PyErr_Occurred())
            goto finally;
        *offset = hold;
    }

    v = _PyObject_GetAttrId(err, &PyId_text);
    if (!v)
        goto finally;
    if (v == Py_None) {
        Py_DECREF(v);
        *text = NULL;
    }
    else {
        *text = v;
    }
    return 1;

finally:
    Py_XDECREF(*message);
    Py_XDECREF(*filename);
    return 0;
}
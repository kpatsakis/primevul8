do_curl_getattro(PyObject *o, PyObject *n)
{
    PyObject *v = PyObject_GenericGetAttr(o, n);
    if( !v && PyErr_ExceptionMatches(PyExc_AttributeError) )
    {
        PyErr_Clear();
        v = my_getattro(o, n, ((CurlObject *)o)->dict,
                        curlobject_constants, curlobject_methods);
    }
    return v;
}
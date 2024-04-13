builtin_issubclass_impl(PyObject *module, PyObject *cls,
                        PyObject *class_or_tuple)
/*[clinic end generated code: output=358412410cd7a250 input=af5f35e9ceaddaf6]*/
{
    int retval;

    retval = PyObject_IsSubclass(cls, class_or_tuple);
    if (retval < 0)
        return NULL;
    return PyBool_FromLong(retval);
}
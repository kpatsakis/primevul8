builtin_isinstance_impl(PyObject *module, PyObject *obj,
                        PyObject *class_or_tuple)
/*[clinic end generated code: output=6faf01472c13b003 input=ffa743db1daf7549]*/
{
    int retval;

    retval = PyObject_IsInstance(obj, class_or_tuple);
    if (retval < 0)
        return NULL;
    return PyBool_FromLong(retval);
}
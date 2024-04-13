builtin_hash(PyObject *module, PyObject *obj)
/*[clinic end generated code: output=237668e9d7688db7 input=58c48be822bf9c54]*/
{
    Py_hash_t x;

    x = PyObject_Hash(obj);
    if (x == -1)
        return NULL;
    return PyLong_FromSsize_t(x);
}
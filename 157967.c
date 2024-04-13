builtin_callable(PyObject *module, PyObject *obj)
/*[clinic end generated code: output=2b095d59d934cb7e input=1423bab99cc41f58]*/
{
    return PyBool_FromLong((long)PyCallable_Check(obj));
}
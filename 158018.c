builtin_bin(PyObject *module, PyObject *number)
/*[clinic end generated code: output=b6fc4ad5e649f4f7 input=53f8a0264bacaf90]*/
{
    return PyNumber_ToBase(number, 2);
}
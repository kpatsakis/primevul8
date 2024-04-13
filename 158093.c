builtin_hex(PyObject *module, PyObject *number)
/*[clinic end generated code: output=e46b612169099408 input=e645aff5fc7d540e]*/
{
    return PyNumber_ToBase(number, 16);
}
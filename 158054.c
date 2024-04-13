builtin_oct(PyObject *module, PyObject *number)
/*[clinic end generated code: output=40a34656b6875352 input=ad6b274af4016c72]*/
{
    return PyNumber_ToBase(number, 8);
}
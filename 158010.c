filter_reduce(filterobject *lz, PyObject *Py_UNUSED(ignored))
{
    return Py_BuildValue("O(OO)", Py_TYPE(lz), lz->func, lz->it);
}
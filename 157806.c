zip_reduce(zipobject *lz, PyObject *Py_UNUSED(ignored))
{
    /* Just recreate the zip with the internal iterator tuple */
    return Py_BuildValue("OO", Py_TYPE(lz), lz->ittuple);
}
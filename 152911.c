PyObject* ast2obj_boolop(boolop_ty o)
{
    switch(o) {
        case And:
            Py_INCREF(And_singleton);
            return And_singleton;
        case Or:
            Py_INCREF(Or_singleton);
            return Or_singleton;
        default:
            /* should never happen, but just in case ... */
            PyErr_Format(PyExc_SystemError, "unknown boolop found");
            return NULL;
    }
}
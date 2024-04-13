PyObject* ast2obj_unaryop(unaryop_ty o)
{
    switch(o) {
        case Invert:
            Py_INCREF(Invert_singleton);
            return Invert_singleton;
        case Not:
            Py_INCREF(Not_singleton);
            return Not_singleton;
        case UAdd:
            Py_INCREF(UAdd_singleton);
            return UAdd_singleton;
        case USub:
            Py_INCREF(USub_singleton);
            return USub_singleton;
        default:
            /* should never happen, but just in case ... */
            PyErr_Format(PyExc_SystemError, "unknown unaryop found");
            return NULL;
    }
}
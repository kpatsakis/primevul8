PyObject* ast2obj_expr_context(expr_context_ty o)
{
    switch(o) {
        case Load:
            Py_INCREF(Load_singleton);
            return Load_singleton;
        case Store:
            Py_INCREF(Store_singleton);
            return Store_singleton;
        case Del:
            Py_INCREF(Del_singleton);
            return Del_singleton;
        case AugLoad:
            Py_INCREF(AugLoad_singleton);
            return AugLoad_singleton;
        case AugStore:
            Py_INCREF(AugStore_singleton);
            return AugStore_singleton;
        case Param:
            Py_INCREF(Param_singleton);
            return Param_singleton;
        case NamedStore:
            Py_INCREF(NamedStore_singleton);
            return NamedStore_singleton;
        default:
            /* should never happen, but just in case ... */
            PyErr_Format(PyExc_SystemError, "unknown expr_context found");
            return NULL;
    }
}
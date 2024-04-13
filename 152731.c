PyObject* ast2obj_cmpop(cmpop_ty o)
{
    switch(o) {
        case Eq:
            Py_INCREF(Eq_singleton);
            return Eq_singleton;
        case NotEq:
            Py_INCREF(NotEq_singleton);
            return NotEq_singleton;
        case Lt:
            Py_INCREF(Lt_singleton);
            return Lt_singleton;
        case LtE:
            Py_INCREF(LtE_singleton);
            return LtE_singleton;
        case Gt:
            Py_INCREF(Gt_singleton);
            return Gt_singleton;
        case GtE:
            Py_INCREF(GtE_singleton);
            return GtE_singleton;
        case Is:
            Py_INCREF(Is_singleton);
            return Is_singleton;
        case IsNot:
            Py_INCREF(IsNot_singleton);
            return IsNot_singleton;
        case In:
            Py_INCREF(In_singleton);
            return In_singleton;
        case NotIn:
            Py_INCREF(NotIn_singleton);
            return NotIn_singleton;
        default:
            /* should never happen, but just in case ... */
            PyErr_Format(PyExc_SystemError, "unknown cmpop found");
            return NULL;
    }
}
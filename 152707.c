PyObject* ast2obj_operator(operator_ty o)
{
    switch(o) {
        case Add:
            Py_INCREF(Add_singleton);
            return Add_singleton;
        case Sub:
            Py_INCREF(Sub_singleton);
            return Sub_singleton;
        case Mult:
            Py_INCREF(Mult_singleton);
            return Mult_singleton;
        case MatMult:
            Py_INCREF(MatMult_singleton);
            return MatMult_singleton;
        case Div:
            Py_INCREF(Div_singleton);
            return Div_singleton;
        case Mod:
            Py_INCREF(Mod_singleton);
            return Mod_singleton;
        case Pow:
            Py_INCREF(Pow_singleton);
            return Pow_singleton;
        case LShift:
            Py_INCREF(LShift_singleton);
            return LShift_singleton;
        case RShift:
            Py_INCREF(RShift_singleton);
            return RShift_singleton;
        case BitOr:
            Py_INCREF(BitOr_singleton);
            return BitOr_singleton;
        case BitXor:
            Py_INCREF(BitXor_singleton);
            return BitXor_singleton;
        case BitAnd:
            Py_INCREF(BitAnd_singleton);
            return BitAnd_singleton;
        case FloorDiv:
            Py_INCREF(FloorDiv_singleton);
            return FloorDiv_singleton;
        default:
            /* should never happen, but just in case ... */
            PyErr_Format(PyExc_SystemError, "unknown operator found");
            return NULL;
    }
}
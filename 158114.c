validate_slice(slice_ty slice)
{
    switch (slice->kind) {
    case Slice_kind:
        return (!slice->v.Slice.lower || validate_expr(slice->v.Slice.lower, Load)) &&
            (!slice->v.Slice.upper || validate_expr(slice->v.Slice.upper, Load)) &&
            (!slice->v.Slice.step || validate_expr(slice->v.Slice.step, Load));
    case ExtSlice_kind: {
        Py_ssize_t i;
        if (!validate_nonempty_seq(slice->v.ExtSlice.dims, "dims", "ExtSlice"))
            return 0;
        for (i = 0; i < asdl_seq_LEN(slice->v.ExtSlice.dims); i++)
            if (!validate_slice(asdl_seq_GET(slice->v.ExtSlice.dims, i)))
                return 0;
        return 1;
    }
    case Index_kind:
        return validate_expr(slice->v.Index.value, Load);
    default:
        PyErr_SetString(PyExc_SystemError, "unknown slice node");
        return 0;
    }
}
ExprList_Finish(ExprList *l, PyArena *arena)
{
    asdl_seq *seq;

    ExprList_check_invariants(l);

    /* Allocate the asdl_seq and copy the expressions in to it. */
    seq = _Py_asdl_seq_new(l->size, arena);
    if (seq) {
        Py_ssize_t i;
        for (i = 0; i < l->size; i++)
            asdl_seq_SET(seq, i, l->p[i]);
    }
    ExprList_Dealloc(l);
    return seq;
}
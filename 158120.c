push(stack *s, int type, dfa *d, int newstate, int lineno, int col_offset,
     int end_lineno, int end_col_offset)
{
    int err;
    node *n;
    n = s->s_top->s_parent;
    assert(!s_empty(s));
    err = PyNode_AddChild(n, type, (char *)NULL, lineno, col_offset,
                          end_lineno, end_col_offset);
    if (err)
        return err;
    s->s_top->s_state = newstate;
    return s_push(s, d, CHILD(n, NCH(n)-1));
}
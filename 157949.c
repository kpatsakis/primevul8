shift(stack *s, int type, char *str, int newstate, int lineno, int col_offset,
      int end_lineno, int end_col_offset)
{
    int err;
    assert(!s_empty(s));
    err = PyNode_AddChild(s->s_top->s_parent, type, str, lineno, col_offset,
                          end_lineno, end_col_offset);
    if (err)
        return err;
    s->s_top->s_state = newstate;
    return 0;
}
s_pop(stack *s)
{
    if (s_empty(s))
        Py_FatalError("s_pop: parser stack underflow -- FATAL");
    s->s_top++;
}
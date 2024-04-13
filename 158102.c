s_reset(stack *s)
{
    s->s_top = &s->s_base[MAXSTACK];
}
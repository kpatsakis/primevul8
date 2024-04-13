s_push(stack *s, dfa *d, node *parent)
{
    stackentry *top;
    if (s->s_top == s->s_base) {
        fprintf(stderr, "s_push: parser stack overflow\n");
        return E_NOMEM;
    }
    top = --s->s_top;
    top->s_dfa = d;
    top->s_parent = parent;
    top->s_state = 0;
    return 0;
}
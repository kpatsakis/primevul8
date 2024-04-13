PyParser_New(grammar *g, int start)
{
    parser_state *ps;

    if (!g->g_accel)
        PyGrammar_AddAccelerators(g);
    ps = (parser_state *)PyMem_MALLOC(sizeof(parser_state));
    if (ps == NULL)
        return NULL;
    ps->p_grammar = g;
#ifdef PY_PARSER_REQUIRES_FUTURE_KEYWORD
    ps->p_flags = 0;
#endif
    ps->p_tree = PyNode_New(start);
    if (ps->p_tree == NULL) {
        PyMem_FREE(ps);
        return NULL;
    }
    s_reset(&ps->p_stack);
    (void) s_push(&ps->p_stack, PyGrammar_FindDFA(g, start), ps->p_tree);
    return ps;
}
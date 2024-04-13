Ta3Grammar_FindDFA(grammar *g, int type)
{
    dfa *d;
#if 1
    /* Massive speed-up */
    d = &g->g_dfa[type - NT_OFFSET];
    assert(d->d_type == type);
    return d;
#else
    /* Old, slow version */
    int i;

    for (i = g->g_ndfas, d = g->g_dfa; --i >= 0; d++) {
        if (d->d_type == type)
            return d;
    }
    abort();
#endif
}
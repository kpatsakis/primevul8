printtree(parser_state *ps)
{
    if (Py_DebugFlag) {
        printf("Parse tree:\n");
        dumptree(ps->p_grammar, ps->p_tree);
        printf("\n");
        printf("Tokens:\n");
        showtree(ps->p_grammar, ps->p_tree);
        printf("\n");
    }
    printf("Listing:\n");
    PyNode_ListTree(ps->p_tree);
    printf("\n");
}
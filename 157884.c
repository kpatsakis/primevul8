PyParser_Delete(parser_state *ps)
{
    /* NB If you want to save the parse tree,
       you must set p_tree to NULL before calling delparser! */
    PyNode_Free(ps->p_tree);
    PyMem_FREE(ps);
}
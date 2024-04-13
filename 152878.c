ast_for_testlist_comp(struct compiling *c, const node* n)
{
    /* testlist_comp: test ( comp_for | (',' test)* [','] ) */
    /* argument: test [ comp_for ] */
    assert(TYPE(n) == testlist_comp || TYPE(n) == argument);
    if (NCH(n) > 1 && TYPE(CHILD(n, 1)) == comp_for)
        return ast_for_genexp(c, n);
    return ast_for_testlist(c, n);
}
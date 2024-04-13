count_list_ifs(struct compiling *c, const node *n)
{
    int n_ifs = 0;

 count_list_iter:
    REQ(n, list_iter);
    if (TYPE(CHILD(n, 0)) == list_for)
        return n_ifs;
    n = CHILD(n, 0);
    REQ(n, list_if);
    n_ifs++;
    if (NCH(n) == 2)
        return n_ifs;
    n = CHILD(n, 2);
    goto count_list_iter;
}
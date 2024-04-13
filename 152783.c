count_list_fors(struct compiling *c, const node *n)
{
    int n_fors = 0;
    node *ch = CHILD(n, 1);

 count_list_for:
    n_fors++;
    REQ(ch, list_for);
    if (NCH(ch) == 5)
        ch = CHILD(ch, 4);
    else
        return n_fors;
 count_list_iter:
    REQ(ch, list_iter);
    ch = CHILD(ch, 0);
    if (TYPE(ch) == list_for)
        goto count_list_for;
    else if (TYPE(ch) == list_if) {
        if (NCH(ch) == 3) {
            ch = CHILD(ch, 2);
            goto count_list_iter;
        }
        else
            return n_fors;
    }

    /* Should never be reached */
    PyErr_SetString(PyExc_SystemError, "logic error in count_list_fors");
    return -1;
}
count_comp_fors(struct compiling *c, const node *n)
{
    int n_fors = 0;

  count_comp_for:
    n_fors++;
    REQ(n, comp_for);
    if (NCH(n) == 5)
        n = CHILD(n, 4);
    else
        return n_fors;
  count_comp_iter:
    REQ(n, comp_iter);
    n = CHILD(n, 0);
    if (TYPE(n) == comp_for)
        goto count_comp_for;
    else if (TYPE(n) == comp_if) {
        if (NCH(n) == 3) {
            n = CHILD(n, 2);
            goto count_comp_iter;
        }
        else
            return n_fors;
    }

    /* Should never be reached */
    PyErr_SetString(PyExc_SystemError,
                    "logic error in count_comp_fors");
    return -1;
}
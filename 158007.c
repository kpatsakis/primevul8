parser_compare_nodes(node *left, node *right)
{
    int j;

    if (TYPE(left) < TYPE(right))
        return (-1);

    if (TYPE(right) < TYPE(left))
        return (1);

    if (ISTERMINAL(TYPE(left)))
        return (strcmp(STR(left), STR(right)));

    if (NCH(left) < NCH(right))
        return (-1);

    if (NCH(right) < NCH(left))
        return (1);

    for (j = 0; j < NCH(left); ++j) {
        int v = parser_compare_nodes(CHILD(left, j), CHILD(right, j));

        if (v != 0)
            return (v);
    }
    return (0);
}
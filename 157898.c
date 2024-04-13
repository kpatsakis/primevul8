showtree(grammar *g, node *n)
{
    int i;

    if (n == NULL)
        return;
    if (ISNONTERMINAL(TYPE(n))) {
        for (i = 0; i < NCH(n); i++)
            showtree(g, CHILD(n, i));
    }
    else if (ISTERMINAL(TYPE(n))) {
        printf("%s", _PyParser_TokenNames[TYPE(n)]);
        if (TYPE(n) == NUMBER || TYPE(n) == NAME)
            printf("(%s)", STR(n));
        printf(" ");
    }
    else
        printf("? ");
}
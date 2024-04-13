dumptree(grammar *g, node *n)
{
    int i;

    if (n == NULL)
        printf("NIL");
    else {
        label l;
        l.lb_type = TYPE(n);
        l.lb_str = STR(n);
        printf("%s", PyGrammar_LabelRepr(&l));
        if (ISNONTERMINAL(TYPE(n))) {
            printf("(");
            for (i = 0; i < NCH(n); i++) {
                if (i > 0)
                    printf(",");
                dumptree(g, CHILD(n, i));
            }
            printf(")");
        }
    }
}
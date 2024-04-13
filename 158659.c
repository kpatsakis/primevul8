debug_print_tree(tree_node *p)
{
int i;
for (i = 0; i < tree_printlinesize; i++) tree_printline[i] = ' ';
if (!p) debug_printf("Empty Tree\n"); else tree_printsub(p, 0, 0);
debug_printf("---- End of tree ----\n");
}
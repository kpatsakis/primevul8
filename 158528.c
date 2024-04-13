tree_printsub(tree_node *p, int pos, int barswitch)
{
int i;
if (p->right) tree_printsub(p->right, pos+2, 1);
for (i = 0; i <= pos-1; i++) debug_printf("%c", tree_printline[i]);
debug_printf("-->%s [%d]\n", p->name, p->balance);
tree_printline[pos] = barswitch? '|' : ' ';
if (p->left)
  {
  tree_printline[pos+2] = '|';
  tree_printsub(p->left, pos+2, 0);
  }
}
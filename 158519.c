acl_var_create(uschar * name)
{
tree_node * node, ** root = name[0] == 'c' ? &acl_var_c : &acl_var_m;
if (!(node = tree_search(*root, name)))
  {
  node = store_get(sizeof(tree_node) + Ustrlen(name));
  Ustrcpy(node->name, name);
  (void)tree_insertnode(root, node);
  }
node->data.ptr = NULL;
return node;
}
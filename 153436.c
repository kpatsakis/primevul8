onig_new_cclass_with_code_list(Node** rnode, OnigEncoding enc,
                               int n, OnigCodePoint codes[])
{
  int i;
  Node* node;
  CClassNode* cc;

  *rnode = NULL_NODE;

  node = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(node);

  cc = CCLASS_(node);

  for (i = 0; i < n; i++) {
    ADD_CODE_INTO_CC(cc, codes[i], enc);
  }

  *rnode = node;
  return 0;
}
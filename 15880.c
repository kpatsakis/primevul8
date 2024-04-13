_asn1_find_up (asn1_node node)
{
  asn1_node p;

  if (node == NULL)
    return NULL;

  p = node;

  while ((p->left != NULL) && (p->left->right == p))
    p = p->left;

  return p->left;
}
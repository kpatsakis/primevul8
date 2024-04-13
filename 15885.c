_asn1_get_last_right (asn1_node node)
{
  asn1_node p;

  if (node == NULL)
    return NULL;
  p = node;
  while (p->right)
    p = p->right;
  return p;
}
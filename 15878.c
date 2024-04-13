_asn1_set_right (asn1_node node, asn1_node right)
{
  if (node == NULL)
    return node;
  node->right = right;
  if (right)
    {
      right->left = node;
      if (right->up == NULL)
        right->up = node->up;
    }
  return node;
}
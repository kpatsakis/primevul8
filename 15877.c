_asn1_set_value_m (asn1_node node, void *value, unsigned int len)
{
  if (node == NULL)
    return node;

  if (node->value)
    {
      if (node->value != node->small_value)
	free (node->value);
      node->value = NULL;
      node->value_len = 0;
    }

  if (!len)
    return node;

  node->value = value;
  node->value_len = len;

  return node;
}
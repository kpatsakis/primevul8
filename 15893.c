_asn1_set_value (asn1_node node, const void *value, unsigned int len)
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

  if (len < sizeof (node->small_value))
    {
      node->value = node->small_value;
    }
  else
    {
      node->value = malloc (len);
      if (node->value == NULL)
	return NULL;
    }
  node->value_len = len;

  memcpy (node->value, value, len);
  return node;
}
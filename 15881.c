_asn1_remove_node (asn1_node node, unsigned int flags)
{
  if (node == NULL)
    return;

  if (node->value != NULL)
    {
      if (flags & ASN1_DELETE_FLAG_ZEROIZE)
        {
          safe_memset(node->value, 0, node->value_len);
        }

      if (node->value != node->small_value)
        free (node->value);
    }
  free (node);
}
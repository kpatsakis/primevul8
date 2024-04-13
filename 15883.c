_asn1_set_name (asn1_node node, const char *name)
{
  unsigned int nsize;

  if (node == NULL)
    return node;

  if (name == NULL)
    {
      node->name[0] = 0;
      node->name_hash = hash_pjw_bare (node->name, 0);
      return node;
    }

  nsize = _asn1_str_cpy (node->name, sizeof (node->name), name);
  node->name_hash = hash_pjw_bare (node->name, nsize);

  return node;
}
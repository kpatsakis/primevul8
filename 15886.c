_asn1_cpy_name (asn1_node dst, asn1_node src)
{
  if (dst == NULL)
    return dst;

  if (src == NULL)
    {
      dst->name[0] = 0;
      dst->name_hash = hash_pjw_bare (dst->name, 0);
      return dst;
    }

  _asn1_str_cpy (dst->name, sizeof (dst->name), src->name);
  dst->name_hash = src->name_hash;

  return dst;
}
_asn1_append_sequence_set (asn1_node node)
{
  asn1_node p, p2;
  char temp[10];
  long n;

  if (!node || !(node->down))
    return ASN1_GENERIC_ERROR;

  p = node->down;
  while ((type_field (p->type) == ASN1_ETYPE_TAG)
	 || (type_field (p->type) == ASN1_ETYPE_SIZE))
    p = p->right;
  p2 = _asn1_copy_structure3 (p);
  while (p->right)
    p = p->right;
  _asn1_set_right (p, p2);

  if (p->name[0] == 0)
    _asn1_str_cpy (temp, sizeof (temp), "?1");
  else
    {
      n = strtol (p->name + 1, NULL, 0);
      n++;
      temp[0] = '?';
      _asn1_ltostr (n, temp + 1);
    }
  _asn1_set_name (p2, temp);
  /*  p2->type |= CONST_OPTION; */

  return ASN1_SUCCESS;
}
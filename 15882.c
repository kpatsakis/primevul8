_asn1_set_value_lv (asn1_node node, const void *value, unsigned int len)
{
  int len2;
  void *temp;

  if (node == NULL)
    return node;

  asn1_length_der (len, NULL, &len2);
  temp = malloc (len + len2);
  if (temp == NULL)
    return NULL;

  asn1_octet_der (value, len, temp, &len2);
  return _asn1_set_value_m (node, temp, len2);
}
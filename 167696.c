asn1_read_value (asn1_node root, const char *name, void *ivalue, int *len)
{
  return asn1_read_value_type (root, name, ivalue, len, NULL);
}
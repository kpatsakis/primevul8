asn1_read_node_value (asn1_node node, asn1_data_node_st * data)
{
  data->name = node->name;
  data->value = node->value;
  data->value_len = node->value_len;
  data->type = type_field (node->type);

  return ASN1_SUCCESS;
}
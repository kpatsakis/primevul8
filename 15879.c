_asn1_add_static_node (unsigned int type)
{
  list_type *listElement;
  asn1_node punt;

  punt = calloc (1, sizeof (struct asn1_node_st));
  if (punt == NULL)
    return NULL;

  listElement = malloc (sizeof (list_type));
  if (listElement == NULL)
    {
      free (punt);
      return NULL;
    }

  listElement->node = punt;
  listElement->next = firstElement;
  firstElement = listElement;

  punt->type = type;

  return punt;
}
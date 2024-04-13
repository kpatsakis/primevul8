_asn1_delete_list_and_nodes (void)
{
  list_type *listElement;

  while (firstElement)
    {
      listElement = firstElement;
      firstElement = firstElement->next;
      _asn1_remove_node (listElement->node, 0);
      free (listElement);
    }
}
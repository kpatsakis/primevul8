_asn1_delete_list (void)
{
  list_type *listElement;

  while (firstElement)
    {
      listElement = firstElement;
      firstElement = firstElement->next;
      free (listElement);
    }
}
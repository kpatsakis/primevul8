host_remove_duplicates(host_item *host, host_item **lastptr)
{
while (host != *lastptr)
  {
  if (host->address != NULL)
    {
    host_item *h = host;
    while (h != *lastptr)
      {
      if (h->next->address != NULL &&
          Ustrcmp(h->next->address, host->address) == 0)
        {
        DEBUG(D_host_lookup) debug_printf("duplicate IP address %s (MX=%d) "
          "removed\n", host->address, h->next->mx);
        if (h->next == *lastptr) *lastptr = h;
        h->next = h->next->next;
        }
      else h = h->next;
      }
    }
  /* If the last item was removed, host may have become == *lastptr */
  if (host != *lastptr) host = host->next;
  }
}
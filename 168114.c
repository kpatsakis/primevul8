compare_orports_(const void **_a, const void **_b)
{
  const tor_addr_port_t *a = *_a, *b = *_b;
  int r;

  if ((r = tor_addr_compare(&a->addr, &b->addr, CMP_EXACT)))
    return r;
  if ((r = (((int) b->port) - ((int) a->port))))
    return r;

  return 0;
}
hivex_close (hive_h *h)
{
  int r;

  DEBUG (1, "hivex_close");

  free (h->bitmap);
  if (!h->writable)
    munmap (h->addr, h->size);
  else
    free (h->addr);
  if (h->fd >= 0)
    r = close (h->fd);
  else
    r = 0;
  free (h->filename);
  free (h);

  return r;
}
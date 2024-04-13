psp_zalloc (void  *opaque,
            guint  items,
            guint  size)
{
  return g_malloc (items*size);
}
str_case_hash (gconstpointer key)
{
  const char *p = key;
  guint h = g_ascii_toupper (*p);

  if (h)
    for (p += 1; *p != '\0'; p++)
      h = (h << 5) - h + g_ascii_toupper (*p);

  return h;
}
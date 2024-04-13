str_case_equal (gconstpointer v1, gconstpointer v2)
{
  const char *string1 = v1;
  const char *string2 = v2;

  return g_ascii_strcasecmp (string1, string2) == 0;
}
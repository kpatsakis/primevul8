join_args (GPtrArray *argv_array, gsize *len_out)
{
  gchar *string;
  gchar *ptr;
  gint i;
  gsize len = 0;

  for (i = 0; i < argv_array->len; i++)
    len +=  strlen (argv_array->pdata[i]) + 1;

  string = g_new (gchar, len);
  *string = 0;
  ptr = string;
  for (i = 0; i < argv_array->len; i++)
    ptr = g_stpcpy (ptr, argv_array->pdata[i]) + 1;

  *len_out = len;
  return string;
}
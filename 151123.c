add_args (GPtrArray *argv_array, ...)
{
  va_list args;
  const gchar *arg;

  va_start (args, argv_array);
  while ((arg = va_arg (args, const gchar *)))
    g_ptr_array_add (argv_array, g_strdup (arg));
  va_end (args);
}
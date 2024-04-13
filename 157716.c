channel_type_name (gint type)
{
  static const gchar *channel_type_names[] =
  {
    "COMPOSITE",
    "RED",
    "GREEN",
    "BLUE"
  };
  static gchar *err_name = NULL;

  if (type >= 0 && type <= PSP_CHANNEL_BLUE)
    return channel_type_names[type];

  g_free (err_name);
  err_name = g_strdup_printf ("unknown channel type %d", type);

  return err_name;
}
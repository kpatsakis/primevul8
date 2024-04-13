bitmap_type_name (gint type)
{
  static const gchar *bitmap_type_names[] =
  {
    "IMAGE",
    "TRANS_MASK",
    "USER_MASK",
    "SELECTION",
    "ALPHA_MASK",
    "THUMBNAIL"
  };
  static gchar *err_name = NULL;

  if (type >= 0 && type <= PSP_DIB_THUMBNAIL)
    return bitmap_type_names[type];

  g_free (err_name);
  err_name = g_strdup_printf ("unknown bitmap type %d", type);

  return err_name;
}
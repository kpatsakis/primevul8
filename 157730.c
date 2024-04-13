block_name (gint id)
{
  static const gchar *block_names[] =
  {
    "IMAGE",
    "CREATOR",
    "COLOR",
    "LAYER_START",
    "LAYER",
    "CHANNEL",
    "SELECTION",
    "ALPHA_BANK",
    "ALPHA_CHANNEL",
    "THUMBNAIL",
    "EXTENDED_DATA",
    "TUBE",
    "ADJUSTMENT_EXTENSION",
    "VECTOR_EXTENSION_BLOCK",
    "SHAPE_BLOCK",
    "PAINTSTYLE_BLOCK",
    "COMPOSITE_IMAGE_BANK_BLOCK",
    "COMPOSITE_ATTRIBUTES_BLOCK",
    "JPEG_BLOCK",
  };
  static gchar *err_name = NULL;

  if (id >= 0 && id <= PSP_JPEG_BLOCK)
    return block_names[id];

  g_free (err_name);
  err_name = g_strdup_printf ("id=%d", id);

  return err_name;
}
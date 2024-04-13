kate_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 64);
  gchar category[16] = { 0, };

  if (G_UNLIKELY (data == NULL))
    return;

  /* see: http://wiki.xiph.org/index.php/OggKate#Format_specification */
  if (G_LIKELY (memcmp (data, "\200kate\0\0\0", 8) != 0))
    return;

  /* make sure we always have a NUL-terminated string */
  memcpy (category, data + 48, 15);
  GST_LOG ("kate category: %s", category);
  /* canonical categories for subtitles: subtitles, spu-subtitles, SUB, K-SPU */
  if (strcmp (category, "subtitles") == 0 || strcmp (category, "SUB") == 0 ||
      strcmp (category, "spu-subtitles") == 0 ||
      strcmp (category, "K-SPU") == 0) {
    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM,
        "subtitle/x-kate", NULL);
  } else {
    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM,
        "application/x-kate", NULL);
  }
}
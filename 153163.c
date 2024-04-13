degas_type_find (GstTypeFind * tf, gpointer private)
{
  /* No magic, but it should have a fixed size and a few invalid values */
  /* http://www.fileformat.info/format/atari/spec/6ecf9f6eb5be494284a47feb8a214687/view.htm */
  gint64 len;
  const guint8 *data;
  guint16 resolution;
  int n;

  len = gst_type_find_get_length (tf);
  if (len < 34)                 /* smallest header of the lot */
    return;
  data = gst_type_find_peek (tf, 0, 4);
  if (G_UNLIKELY (data == NULL))
    return;
  resolution = GST_READ_UINT16_BE (data);
  if (len == 32034) {
    /* could be DEGAS */
    if (resolution <= 2)
      gst_type_find_suggest_simple (tf, GST_TYPE_FIND_POSSIBLE + 5,
          "image/x-degas", NULL);
  } else if (len == 32066) {
    /* could be DEGAS Elite */
    if (resolution <= 2) {
      data = gst_type_find_peek (tf, len - 16, 8);
      if (G_UNLIKELY (data == NULL))
        return;
      for (n = 0; n < 4; n++) {
        if (GST_READ_UINT16_BE (data + n * 2) > 2)
          return;
      }
      gst_type_find_suggest_simple (tf, GST_TYPE_FIND_POSSIBLE + 5,
          "image/x-degas", NULL);
    }
  } else if (len >= 66 && len < 32066) {
    /* could be compressed DEGAS Elite, but it's compressed and so we can't rely on size,
       it does have 4 16 bytes values near the end that are 0-2 though. */
    if ((resolution & 0x8000) && (resolution & 0x7fff) <= 2) {
      data = gst_type_find_peek (tf, len - 16, 8);
      if (G_UNLIKELY (data == NULL))
        return;
      for (n = 0; n < 4; n++) {
        if (GST_READ_UINT16_BE (data + n * 2) > 2)
          return;
      }
      gst_type_find_suggest_simple (tf, GST_TYPE_FIND_POSSIBLE + 5,
          "image/x-degas", NULL);
    }
  }
}
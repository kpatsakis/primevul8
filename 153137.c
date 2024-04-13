sbc_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;
  gsize frame_len;
  guint i, rate, channels, offset = 0;

  for (i = 0; i < 10; ++i) {
    data = gst_type_find_peek (tf, offset, 8);
    if (data == NULL)
      return;

    frame_len = sbc_check_header (data, 8, &rate, &channels);
    if (frame_len == 0)
      return;

    offset += frame_len;
  }
  gst_type_find_suggest_simple (tf, GST_TYPE_FIND_POSSIBLE, "audio/x-sbc",
      "rate", G_TYPE_INT, rate, "channels", G_TYPE_INT, channels,
      "parsed", G_TYPE_BOOLEAN, FALSE, NULL);
}
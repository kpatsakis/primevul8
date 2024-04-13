dv_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 5);

  /* check for DIF  and DV flag */
  if (data && (data[0] == 0x1f) && (data[1] == 0x07) && (data[2] == 0x00)) {
    const gchar *format;

    if (data[3] & 0x80) {
      format = "PAL";
    } else {
      format = "NTSC";
    }

    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM, "video/x-dv",
        "systemstream", G_TYPE_BOOLEAN, TRUE,
        "format", G_TYPE_STRING, format, NULL);
  }
}
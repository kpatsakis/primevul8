y4m_typefind (GstTypeFind * tf, gpointer private)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 10);
  if (data != NULL && memcmp (data, "YUV4MPEG2 ", 10) == 0) {
    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_LIKELY,
        "application/x-yuv4mpeg", "y4mversion", G_TYPE_INT, 2, NULL);
  }
}
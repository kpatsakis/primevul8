ar_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 24);

  if (data && memcmp (data, "!<arch>", 7) == 0) {
    gint i;

    for (i = 7; i < 24; ++i) {
      if (!g_ascii_isprint (data[i]) && data[i] != '\n') {
        gst_type_find_suggest (tf, GST_TYPE_FIND_POSSIBLE, AR_CAPS);
      }
    }

    gst_type_find_suggest (tf, GST_TYPE_FIND_NEARLY_CERTAIN, AR_CAPS);
  }
}
dirac_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 8);

  if (data) {
    if (memcmp (data, "BBCD", 4) == 0 || memcmp (data, "KW-DIRAC", 8) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, DIRAC_CAPS);
    }
  }
}
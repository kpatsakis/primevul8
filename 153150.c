paris_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 4);

  if (data) {
    if (memcmp (data, " paf", 4) == 0 || memcmp (data, "fap ", 4) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, PARIS_CAPS);
    }
  }
}
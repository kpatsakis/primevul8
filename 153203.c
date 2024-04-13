apetag_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;

  /* APEv1/2 at start of file */
  data = gst_type_find_peek (tf, 0, 8);
  if (data && !memcmp (data, "APETAGEX", 8)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, APETAG_CAPS);
    return;
  }

  /* APEv1/2 at end of file */
  data = gst_type_find_peek (tf, -32, 8);
  if (data && !memcmp (data, "APETAGEX", 8)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, APETAG_CAPS);
    return;
  }
}
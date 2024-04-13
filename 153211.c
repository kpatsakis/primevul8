id3v1_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, -128, 3);

  if (data && memcmp (data, "TAG", 3) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, ID3_CAPS);
  }
}
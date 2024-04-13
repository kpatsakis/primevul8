m4a_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 4, 8);

  if (data && (memcmp (data, "ftypM4A ", 8) == 0)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, M4A_CAPS);
  }
}
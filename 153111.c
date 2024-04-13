ogmtext_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 9);

  if (data) {
    if (memcmp (data, "\001text\000\000\000\000", 9) != 0)
      return;
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, OGMTEXT_CAPS);
  }
}
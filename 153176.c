ilbc_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 8);

  if (data) {
    if (memcmp (data, "#!iLBC30", 8) == 0 || memcmp (data, "#!iLBC20", 8) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, ILBC_CAPS);
    }
  }
}
ape_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 4);

  if (data && memcmp (data, "MAC ", 4) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY + 10, APE_CAPS);
  }
}
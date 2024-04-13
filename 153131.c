nuv_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 11);

  if (data) {
    if (memcmp (data, "MythTVVideo", 11) == 0
        || memcmp (data, "NuppelVideo", 11) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, NUV_CAPS);
    }
  }
}
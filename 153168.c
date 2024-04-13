musepack_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 4);
  GstTypeFindProbability prop = GST_TYPE_FIND_MINIMUM;
  gint streamversion = -1;

  if (data && memcmp (data, "MP+", 3) == 0) {
    streamversion = 7;
    if ((data[3] & 0x7f) == 7) {
      prop = GST_TYPE_FIND_MAXIMUM;
    } else {
      prop = GST_TYPE_FIND_LIKELY + 10;
    }
  } else if (data && memcmp (data, "MPCK", 4) == 0) {
    streamversion = 8;
    prop = GST_TYPE_FIND_MAXIMUM;
  }

  if (streamversion != -1) {
    gst_type_find_suggest_simple (tf, prop, "audio/x-musepack",
        "streamversion", G_TYPE_INT, streamversion, NULL);
  }
}
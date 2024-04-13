tta_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 3);

  if (data) {
    if (memcmp (data, "TTA", 3) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, TTA_CAPS);
      return;
    }
  }
}
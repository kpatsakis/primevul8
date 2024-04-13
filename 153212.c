sds_type_find (GstTypeFind * tf, gpointer ununsed)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 4);
  guint8 mask[4] = { 0xFF, 0xFF, 0x80, 0xFF };
  guint8 match[4] = { 0xF0, 0x7E, 0, 0x01 };
  gint x;

  if (data) {
    for (x = 0; x < 4; x++) {
      if ((data[x] & mask[x]) != match[x]) {
        return;
      }
    }
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, SDS_CAPS);
  }
}
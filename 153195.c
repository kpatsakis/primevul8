id3v2_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 10);

  if (data && memcmp (data, "ID3", 3) == 0 &&
      data[3] != 0xFF && data[4] != 0xFF &&
      (data[6] & 0x80) == 0 && (data[7] & 0x80) == 0 &&
      (data[8] & 0x80) == 0 && (data[9] & 0x80) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, ID3_CAPS);
  }
}
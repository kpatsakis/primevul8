tiff_type_find (GstTypeFind * tf, gpointer ununsed)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 8);
  guint8 le_header[4] = { 0x49, 0x49, 0x2A, 0x00 };
  guint8 be_header[4] = { 0x4D, 0x4D, 0x00, 0x2A };

  if (data) {
    if (memcmp (data, le_header, 4) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, TIFF_LE_CAPS);
    } else if (memcmp (data, be_header, 4) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, TIFF_BE_CAPS);
    }
  }
}
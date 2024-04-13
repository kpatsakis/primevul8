msdos_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 64);

  if (data && data[0] == 'M' && data[1] == 'Z' &&
      GST_READ_UINT16_LE (data + 8) == 4) {
    guint32 pe_offset = GST_READ_UINT32_LE (data + 60);

    data = gst_type_find_peek (tf, pe_offset, 2);
    if (data && data[0] == 'P' && data[1] == 'E') {
      gst_type_find_suggest (tf, GST_TYPE_FIND_NEARLY_CERTAIN, MSDOS_CAPS);
    }
  }
}
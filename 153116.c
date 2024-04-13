check_utf16 (const guint8 * data, gint len, gint endianness)
{
  GstByteReader br;
  guint16 high, low;

  low = high = 0;

  if (len & 1)
    return FALSE;

  gst_byte_reader_init (&br, data, len);
  while (len >= 2) {
    /* test first for a single 16 bit value in the BMP */
    if (endianness == G_BIG_ENDIAN)
      high = gst_byte_reader_get_uint16_be_unchecked (&br);
    else
      high = gst_byte_reader_get_uint16_le_unchecked (&br);
    if (high >= 0xD800 && high <= 0xDBFF) {
      /* start of a surrogate pair */
      if (len < 4)
        return FALSE;
      len -= 2;
      if (endianness == G_BIG_ENDIAN)
        low = gst_byte_reader_get_uint16_be_unchecked (&br);
      else
        low = gst_byte_reader_get_uint16_le_unchecked (&br);
      if (low >= 0xDC00 && low <= 0xDFFF) {
        /* second half of the surrogate pair */
      } else
        return FALSE;
    } else {
      if (high >= 0xDC00 && high <= 0xDFFF)
        return FALSE;
    }
    len -= 2;
  }
  return TRUE;
}
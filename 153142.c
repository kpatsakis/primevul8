webvtt_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 9);

  if (data == NULL)
    return;

  /* there might be a UTF-8 BOM at the beginning */
  if (memcmp (data, "WEBVTT", 6) != 0 && memcmp (data + 3, "WEBVTT", 6) != 0) {
    return;
  }

  if (data[0] != 'W') {
    if (data[0] != 0xef || data[1] != 0xbb || data[2] != 0xbf)
      return;                   /* Not a UTF-8 BOM */
    data += 3;
  }

  /* After the WEBVTT magic must be one of these chars:
   *   0x20 (space), 0x9 (tab), 0xa (LF) or 0xd (CR) */
  if (data[6] != 0x20 && data[6] != 0x9 && data[6] != 0xa && data[6] != 0xd) {
    return;
  }

  gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, WEBVTT_CAPS);
}
vivo_type_find (GstTypeFind * tf, gpointer unused)
{
  static const guint8 vivo_marker[] = { 'V', 'e', 'r', 's', 'i', 'o', 'n',
    ':', 'V', 'i', 'v', 'o', '/'
  };
  const guint8 *data;
  guint hdr_len, pos;

  data = gst_type_find_peek (tf, 0, 1024);
  if (data == NULL || data[0] != 0x00)
    return;

  if ((data[1] & 0x80)) {
    if ((data[2] & 0x80))
      return;
    hdr_len = ((guint) (data[1] & 0x7f)) << 7;
    hdr_len += data[2];
    if (hdr_len > 2048)
      return;
    pos = 3;
  } else {
    hdr_len = data[1];
    pos = 2;
  }

  /* 1008 = 1022 - strlen ("Version:Vivo/") - 1 */
  while (pos < 1008 && data[pos] == '\r' && data[pos + 1] == '\n')
    pos += 2;

  if (memcmp (data + pos, vivo_marker, sizeof (vivo_marker)) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, VIVO_CAPS);
  }
}
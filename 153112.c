mmsh_type_find (GstTypeFind * tf, gpointer unused)
{
  static const guint8 asf_marker[16] = { 0x30, 0x26, 0xb2, 0x75, 0x8e, 0x66,
    0xcf, 0x11, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c
  };

  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 2 + 2 + 4 + 2 + 2 + 16);
  if (data && data[0] == 0x24 && data[1] == 0x48 &&
      GST_READ_UINT16_LE (data + 2) > 2 + 2 + 4 + 2 + 2 + 16 &&
      memcmp (data + 2 + 2 + 4 + 2 + 2, asf_marker, 16) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, MMSH_CAPS);
  }
}
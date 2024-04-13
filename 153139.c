vorbis_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 30);

  if (data) {
    guint blocksize_0;
    guint blocksize_1;

    /* 1 byte packet type (identification=0x01)
       6 byte string "vorbis"
       4 byte vorbis version */
    if (memcmp (data, "\001vorbis\000\000\000\000", 11) != 0)
      return;
    data += 11;
    /* 1 byte channels must be != 0 */
    if (data[0] == 0)
      return;
    data++;
    /* 4 byte samplerate must be != 0 */
    if (GST_READ_UINT32_LE (data) == 0)
      return;
    data += 16;
    /* blocksize checks */
    blocksize_0 = data[0] & 0x0F;
    blocksize_1 = (data[0] & 0xF0) >> 4;
    if (blocksize_0 > blocksize_1)
      return;
    if (blocksize_0 < 6 || blocksize_0 > 13)
      return;
    if (blocksize_1 < 6 || blocksize_1 > 13)
      return;
    data++;
    /* framing bit */
    if ((data[0] & 0x01) != 1)
      return;
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, VORBIS_CAPS);
  }
}
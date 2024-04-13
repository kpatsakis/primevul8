check_utf32 (const guint8 * data, gint len, gint endianness)
{
  if (len & 3)
    return FALSE;
  while (len > 3) {
    guint32 v;
    if (endianness == G_BIG_ENDIAN)
      v = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    else
      v = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    if (v >= 0x10FFFF)
      return FALSE;
    data += 4;
    len -= 4;
  }
  return TRUE;
}
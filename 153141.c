mpeg_sys_is_valid_pack (GstTypeFind * tf, const guint8 * data, guint len,
    guint * pack_size)
{
  /* Check the pack header @ offset for validity, assuming that the 4 byte header
   * itself has already been checked. */
  guint8 stuff_len;

  if (len < 12)
    return FALSE;

  /* Check marker bits */
  if ((data[4] & 0xC4) == 0x44) {
    /* MPEG-2 PACK */
    if (len < 14)
      return FALSE;

    if ((data[6] & 0x04) != 0x04 ||
        (data[8] & 0x04) != 0x04 ||
        (data[9] & 0x01) != 0x01 || (data[12] & 0x03) != 0x03)
      return FALSE;

    stuff_len = data[13] & 0x07;

    /* Check the following header bytes, if we can */
    if ((14 + stuff_len + 4) <= len) {
      if (!IS_MPEG_HEADER (data + 14 + stuff_len))
        return FALSE;
    }
    if (pack_size)
      *pack_size = 14 + stuff_len;
    return TRUE;
  } else if ((data[4] & 0xF1) == 0x21) {
    /* MPEG-1 PACK */
    if ((data[6] & 0x01) != 0x01 ||
        (data[8] & 0x01) != 0x01 ||
        (data[9] & 0x80) != 0x80 || (data[11] & 0x01) != 0x01)
      return FALSE;

    /* Check the following header bytes, if we can */
    if ((12 + 4) <= len) {
      if (!IS_MPEG_HEADER (data + 12))
        return FALSE;
    }
    if (pack_size)
      *pack_size = 12;
    return TRUE;
  }

  return FALSE;
}
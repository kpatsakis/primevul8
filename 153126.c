mpeg_sys_is_valid_pes (GstTypeFind * tf, const guint8 * data, guint len,
    guint * pack_size)
{
  guint pes_packet_len;

  /* Check the PES header at the given position, assuming the header code itself
   * was already checked */
  if (len < 6)
    return FALSE;

  /* For MPEG Program streams, unbounded PES is not allowed, so we must have a
   * valid length present */
  pes_packet_len = GST_READ_UINT16_BE (data + 4);
  if (pes_packet_len == 0)
    return FALSE;

  /* Check the following header, if we can */
  if (6 + pes_packet_len + 4 <= len) {
    if (!IS_MPEG_HEADER (data + 6 + pes_packet_len))
      return FALSE;
  }

  if (pack_size)
    *pack_size = 6 + pes_packet_len;
  return TRUE;
}
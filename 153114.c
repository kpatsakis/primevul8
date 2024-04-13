dts_parse_frame_header (DataScanCtx * c, guint * frame_size,
    guint * sample_rate, guint * channels, guint * depth, guint * endianness)
{
  static const int sample_rates[16] = { 0, 8000, 16000, 32000, 0, 0, 11025,
    22050, 44100, 0, 0, 12000, 24000, 48000, 96000, 192000
  };
  static const guint8 channels_table[16] = { 1, 2, 2, 2, 2, 3, 3, 4, 4, 5,
    6, 6, 6, 7, 8, 8
  };
  guint16 hdr[8];
  guint32 marker;
  guint num_blocks, chans, lfe, i;

  marker = GST_READ_UINT32_BE (c->data);

  /* raw big endian or 14-bit big endian */
  if (marker == 0x7FFE8001 || marker == 0x1FFFE800) {
    *endianness = G_BIG_ENDIAN;
    for (i = 0; i < G_N_ELEMENTS (hdr); ++i)
      hdr[i] = GST_READ_UINT16_BE (c->data + (i * sizeof (guint16)));
  } else
    /* raw little endian or 14-bit little endian */
  if (marker == 0xFE7F0180 || marker == 0xFF1F00E8) {
    *endianness = G_LITTLE_ENDIAN;
    for (i = 0; i < G_N_ELEMENTS (hdr); ++i)
      hdr[i] = GST_READ_UINT16_LE (c->data + (i * sizeof (guint16)));
  } else {
    return FALSE;
  }

  GST_LOG ("dts sync marker 0x%08x at offset %u", marker, (guint) c->offset);

  /* 14-bit mode */
  if (marker == 0x1FFFE800 || marker == 0xFF1F00E8) {
    if ((hdr[2] & 0xFFF0) != 0x07F0)
      return FALSE;
    /* discard top 2 bits (2 void), shift in 2 */
    hdr[0] = (hdr[0] << 2) | ((hdr[1] >> 12) & 0x0003);
    /* discard top 4 bits (2 void, 2 shifted into hdr[0]), shift in 4 etc. */
    hdr[1] = (hdr[1] << 4) | ((hdr[2] >> 10) & 0x000F);
    hdr[2] = (hdr[2] << 6) | ((hdr[3] >> 8) & 0x003F);
    hdr[3] = (hdr[3] << 8) | ((hdr[4] >> 6) & 0x00FF);
    hdr[4] = (hdr[4] << 10) | ((hdr[5] >> 4) & 0x03FF);
    hdr[5] = (hdr[5] << 12) | ((hdr[6] >> 2) & 0x0FFF);
    hdr[6] = (hdr[6] << 14) | ((hdr[7] >> 0) & 0x3FFF);
    g_assert (hdr[0] == 0x7FFE && hdr[1] == 0x8001);
    *depth = 14;
  } else {
    *depth = 16;
  }

  GST_LOG ("frame header: %04x%04x%04x%04x", hdr[2], hdr[3], hdr[4], hdr[5]);

  num_blocks = (hdr[2] >> 2) & 0x7F;
  *frame_size = (((hdr[2] & 0x03) << 12) | (hdr[3] >> 4)) + 1;
  chans = ((hdr[3] & 0x0F) << 2) | (hdr[4] >> 14);
  *sample_rate = sample_rates[(hdr[4] >> 10) & 0x0F];
  lfe = (hdr[5] >> 9) & 0x03;

  if (num_blocks < 5 || *frame_size < 96 || *sample_rate == 0)
    return FALSE;

  if (marker == 0x1FFFE800 || marker == 0xFF1F00E8)
    *frame_size = (*frame_size * 16) / 14;      /* FIXME: round up? */

  if (chans < G_N_ELEMENTS (channels_table))
    *channels = channels_table[chans] + ((lfe) ? 1 : 0);
  else
    *channels = 0;

  return TRUE;
}
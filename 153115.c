sbc_check_header (const guint8 * data, gsize len, guint * rate,
    guint * channels)
{
  static const guint16 sbc_rates[4] = { 16000, 32000, 44100, 48000 };
  static const guint8 sbc_blocks[4] = { 4, 8, 12, 16 };
  guint n_blocks, ch_mode, n_subbands, bitpool;

  if (data[0] != 0x9C || len < 4)
    return 0;

  n_blocks = sbc_blocks[(data[1] >> 4) & 0x03];
  ch_mode = (data[1] >> 2) & 0x03;
  n_subbands = (data[1] & 0x01) ? 8 : 4;
  bitpool = data[2];
  if (bitpool < 2)
    return 0;

  *rate = sbc_rates[(data[1] >> 6) & 0x03];
  *channels = (ch_mode == 0) ? 1 : 2;

  if (ch_mode == 0)
    return 4 + (n_subbands * 1) / 2 + (n_blocks * 1 * bitpool) / 8;
  else if (ch_mode == 1)
    return 4 + (n_subbands * 2) / 2 + (n_blocks * 2 * bitpool) / 8;
  else if (ch_mode == 2)
    return 4 + (n_subbands * 2) / 2 + (n_blocks * bitpool) / 8;
  else if (ch_mode == 3)
    return 4 + (n_subbands * 2) / 2 + (n_subbands + n_blocks * bitpool) / 8;

  return 0;
}
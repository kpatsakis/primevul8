ebml_read_chunk_header (GstTypeFind * tf, DataScanCtx * c, guint max_size,
    guint32 * id, guint64 * size)
{
  guint64 mask;
  guint msbit_set, i, len, id_len;

  if (c->size < 12 || max_size < 1)
    return 0;

  /* element ID */
  *id = c->data[0];
  if ((c->data[0] & 0x80) == 0x80) {
    id_len = 1;
  } else if ((c->data[0] & 0xC0) == 0x40) {
    id_len = 2;
  } else if ((c->data[0] & 0xE0) == 0x20) {
    id_len = 3;
  } else if ((c->data[0] & 0xF0) == 0x10) {
    id_len = 4;
  } else {
    return 0;
  }

  if (max_size < id_len)
    return 0;

  for (i = 1; i < id_len; ++i) {
    *id = (*id << 8) | c->data[i];
  }

  data_scan_ctx_advance (tf, c, id_len);
  max_size -= id_len;

  /* size */
  if (max_size < 1 || c->data[0] == 0)
    return 0;

  msbit_set = g_bit_nth_msf (c->data[0], 8);
  mask = ((1 << msbit_set) - 1);
  *size = c->data[0] & mask;
  len = 7 - msbit_set;

  if (max_size < 1 + len)
    return 0;
  for (i = 0; i < len; ++i) {
    mask = (mask << 8) | 0xff;
    *size = (*size << 8) | c->data[1 + i];
  }

  data_scan_ctx_advance (tf, c, 1 + len);

  /* undefined/unknown size? (all bits 1) */
  if (*size == mask) {
    /* allow unknown size for SEGMENT chunk, bail out otherwise */
    if (*id == MATROSKA_SEGMENT)
      *size = G_MAXUINT64;
    else
      return 0;
  }

  return id_len + (1 + len);
}
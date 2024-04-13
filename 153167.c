mpeg_find_next_header (GstTypeFind * tf, DataScanCtx * c,
    guint64 max_extra_offset)
{
  guint64 extra_offset;

  for (extra_offset = 0; extra_offset <= max_extra_offset; ++extra_offset) {
    if (!data_scan_ctx_ensure_data (tf, c, 4))
      return FALSE;
    if (IS_MPEG_HEADER (c->data)) {
      data_scan_ctx_advance (tf, c, 3);
      return TRUE;
    }
    data_scan_ctx_advance (tf, c, 1);
  }
  return FALSE;
}
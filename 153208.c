data_scan_ctx_memcmp (GstTypeFind * tf, DataScanCtx * c, guint offset,
    const gchar * data, guint len)
{
  if (!data_scan_ctx_ensure_data (tf, c, offset + len))
    return FALSE;

  return (memcmp (c->data + offset, data, len) == 0);
}
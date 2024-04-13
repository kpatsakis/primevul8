data_scan_ctx_advance (GstTypeFind * tf, DataScanCtx * c, guint bytes_to_skip)
{
  c->offset += bytes_to_skip;
  if (G_LIKELY (c->size > bytes_to_skip)) {
    c->size -= bytes_to_skip;
    c->data += bytes_to_skip;
  } else {
    c->data += c->size;
    c->size = 0;
  }
}
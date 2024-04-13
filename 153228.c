mpeg_ts_probe_headers (GstTypeFind * tf, guint64 offset, gint packet_size)
{
  /* We always enter this function having found at least one header already */
  gint found = 1;
  const guint8 *data = NULL;

  GST_LOG ("looking for mpeg-ts packets of size %u", packet_size);
  while (found < GST_MPEGTS_TYPEFIND_MAX_HEADERS) {
    offset += packet_size;

    data = gst_type_find_peek (tf, offset, MPEGTS_HDR_SIZE);
    if (data == NULL || !IS_MPEGTS_HEADER (data))
      return found;

    found++;
    GST_LOG ("mpeg-ts sync #%2d at offset %" G_GUINT64_FORMAT, found, offset);
  }

  return found;
}
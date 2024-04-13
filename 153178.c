hls_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 7)))
    return;

  if (memcmp (c.data, "#EXTM3U", 7))
    return;

  data_scan_ctx_advance (tf, &c, 7);

  /* Check only the first 256 bytes */
  while (c.offset < 256) {
    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 21)))
      return;

    /* Search for # comment lines */
    if (c.data[0] == '#' && (memcmp (c.data, "#EXT-X-TARGETDURATION", 21) == 0
            || memcmp (c.data, "#EXT-X-STREAM-INF", 17) == 0
            || memcmp (c.data, "#EXT-X-MEDIA", 12) == 0)) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, HLS_CAPS);
      return;
    }

    data_scan_ctx_advance (tf, &c, 1);
  }
}
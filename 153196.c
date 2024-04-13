mpeg_ts_type_find (GstTypeFind * tf, gpointer unused)
{
  /* TS packet sizes to test: normal, DVHS packet size and 
   * FEC with 16 or 20 byte codes packet size. */
  const gint pack_sizes[] = { 188, 192, 204, 208 };
  const guint8 *data = NULL;
  guint size = 0;
  guint64 skipped = 0;

  while (skipped < GST_MPEGTS_TYPEFIND_SCAN_LENGTH) {
    if (size < MPEGTS_HDR_SIZE) {
      data = gst_type_find_peek (tf, skipped, GST_MPEGTS_TYPEFIND_SYNC_SIZE);
      if (!data)
        break;
      size = GST_MPEGTS_TYPEFIND_SYNC_SIZE;
    }

    /* Have at least MPEGTS_HDR_SIZE bytes at this point */
    if (IS_MPEGTS_HEADER (data)) {
      gint p;

      GST_LOG ("possible mpeg-ts sync at offset %" G_GUINT64_FORMAT, skipped);

      for (p = 0; p < G_N_ELEMENTS (pack_sizes); p++) {
        gint found;

        /* Probe ahead at size pack_sizes[p] */
        found = mpeg_ts_probe_headers (tf, skipped, pack_sizes[p]);
        if (found >= GST_MPEGTS_TYPEFIND_MIN_HEADERS) {
          gint probability;

          /* found at least 4 headers. 10 headers = MAXIMUM probability. 
           * Arbitrarily, I assigned 10% probability for each header we
           * found, 40% -> 100% */
          probability = MIN (10 * found, GST_TYPE_FIND_MAXIMUM);

          gst_type_find_suggest_simple (tf, probability, "video/mpegts",
              "systemstream", G_TYPE_BOOLEAN, TRUE,
              "packetsize", G_TYPE_INT, pack_sizes[p], NULL);
          return;
        }
      }
    }
    data++;
    skipped++;
    size--;
  }
}
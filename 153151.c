dts_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };

  /* Search for an dts frame; not necessarily right at the start, but give it
   * a lower probability if not found right at the start. Check that the
   * frame is followed by a second frame at the expected offset. */
  while (c.offset <= DTS_MAX_FRAMESIZE) {
    guint frame_size = 0, rate = 0, chans = 0, depth = 0, endianness = 0;

    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, DTS_MIN_FRAMESIZE)))
      return;

    if (G_UNLIKELY (dts_parse_frame_header (&c, &frame_size, &rate, &chans,
                &depth, &endianness))) {
      GstTypeFindProbability prob;
      DataScanCtx next_c;

      prob = (c.offset == 0) ? GST_TYPE_FIND_LIKELY : GST_TYPE_FIND_POSSIBLE;

      /* check for second frame sync */
      next_c = c;
      data_scan_ctx_advance (tf, &next_c, frame_size);
      if (data_scan_ctx_ensure_data (tf, &next_c, 4)) {
        GST_LOG ("frame size: %u 0x%04x", frame_size, frame_size);
        GST_MEMDUMP ("second frame sync", next_c.data, 4);
        if (GST_READ_UINT32_BE (c.data) == GST_READ_UINT32_BE (next_c.data))
          prob = GST_TYPE_FIND_MAXIMUM;
      }

      if (chans > 0) {
        gst_type_find_suggest_simple (tf, prob, "audio/x-dts",
            "rate", G_TYPE_INT, rate, "channels", G_TYPE_INT, chans,
            "depth", G_TYPE_INT, depth, "endianness", G_TYPE_INT, endianness,
            "framed", G_TYPE_BOOLEAN, FALSE, NULL);
      } else {
        gst_type_find_suggest_simple (tf, prob, "audio/x-dts",
            "rate", G_TYPE_INT, rate, "depth", G_TYPE_INT, depth,
            "endianness", G_TYPE_INT, endianness,
            "framed", G_TYPE_BOOLEAN, FALSE, NULL);
      }

      return;
    }

    data_scan_ctx_advance (tf, &c, 1);
  }
}
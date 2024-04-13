mxf_type_find (GstTypeFind * tf, gpointer ununsed)
{
  static const guint8 partition_pack_key[] =
      { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01, 0x0d, 0x01, 0x02, 0x01,
    0x01
  };
  DataScanCtx c = { 0, NULL, 0 };

  while (c.offset <= MXF_MAX_PROBE_LENGTH) {
    guint i;
    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 1024)))
      break;

    /* look over in chunks of 1kbytes to avoid too much overhead */

    for (i = 0; i < 1024 - 16; i++) {
      /* Check first byte before calling more expensive memcmp function */
      if (G_UNLIKELY (c.data[i] == 0x06
              && memcmp (c.data + i, partition_pack_key, 13) == 0)) {
        /* Header partition pack? */
        if (c.data[i + 13] != 0x02)
          goto advance;

        /* Partition status */
        if (c.data[i + 14] >= 0x05)
          goto advance;

        /* Reserved, must be 0x00 */
        if (c.data[i + 15] != 0x00)
          goto advance;

        gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MXF_CAPS);
        return;
      }
    }

  advance:
    data_scan_ctx_advance (tf, &c, 1024 - 16);
  }
}
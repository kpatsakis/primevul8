itc_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };
  guint8 magic[8] = { 0x00, 0x00, 0x01, 0x1C, 0x69, 0x74, 0x63, 0x68 };
  guint8 preamble[4] = { 0x00, 0x00, 0x00, 0x02 };
  guint8 artwork_marker[8] = { 0x00, 0x00, 0x00, 0x00, 0x61, 0x72, 0x74, 0x77 };
  GstTypeFindProbability itc_prob = GST_TYPE_FIND_NONE;
  int i;

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 8)))
    return;

  if (memcmp (c.data, magic, 8))
    return;

  /* At least we found the right magic */
  itc_prob = GST_TYPE_FIND_MINIMUM;
  data_scan_ctx_advance (tf, &c, 8);

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 12)))
    goto done;

  /* Check preamble 3 consecutive times */
  for (i = 0; i < 3; i++) {
    if (memcmp (c.data, preamble, 4))
      goto done;
    data_scan_ctx_advance (tf, &c, 4);
  }

  itc_prob = GST_TYPE_FIND_POSSIBLE;

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 8)))
    goto done;

  /* Look for "artw" marker */
  if (memcmp (c.data, artwork_marker, 8))
    goto done;

  itc_prob = GST_TYPE_FIND_LIKELY;
  data_scan_ctx_advance (tf, &c, 8);

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 256)))
    goto done;

  /* ...and 256 0x00 padding bytes on what looks like the header's end */
  for (i = 0; i < 256; i++) {
    if (c.data[i])
      goto done;
  }

  itc_prob = GST_TYPE_FIND_NEARLY_CERTAIN;

done:
  gst_type_find_suggest (tf, itc_prob, ITC_CAPS);
}
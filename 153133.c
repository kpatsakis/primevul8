flac_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 4)))
    return;

  /* standard flac (also old/broken flac-in-ogg with an initial 4-byte marker
   * packet and without the usual packet framing) */
  if (memcmp (c.data, "fLaC", 4) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, FLAC_CAPS);
    return;
  }

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 6)))
    return;

  /* flac-in-ogg, see http://flac.sourceforge.net/ogg_mapping.html */
  if (memcmp (c.data, "\177FLAC\001", 6) == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, FLAC_CAPS);
    return;
  }

/* disabled because it happily typefinds /dev/urandom as audio/x-flac, and
 * because I yet have to see header-less flac in the wild */
#if 0
  /* flac without headers (subset format) */
  /* 64K should be enough */
  while (c.offset < (64 * 1024)) {
    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 4)))
      break;

    /* look for frame header,
     * http://flac.sourceforge.net/format.html#frame_header
     */
    if (c.data[0] == 0xff && (c.data[1] >> 2) == 0x3e) {
      /* bit 15 in the header must be 0 */
      if (((c.data[1] >> 1) & 0x01) == 0x01)
        goto advance;

      /* blocksize must be != 0x00 */
      if ((c.data[2] >> 4) == 0x00)
        goto advance;

      /* samplerate must be != 0x0f */
      if ((c.data[2] & 0x0f) == 0x0f)
        goto advance;
      /* also 0 is invalid, as it means get the info from the header and we
       * don't have headers if we are here */
      if ((c.data[2] & 0x0f) == 0x00)
        goto advance;

      /* channel assignment must be < 11 */
      if ((c.data[3] >> 4) >= 11)
        goto advance;

      /* sample size must be != 0x07 and != 0x05 */
      if (((c.data[3] >> 1) & 0x07) == 0x07)
        goto advance;
      if (((c.data[3] >> 1) & 0x07) == 0x05)
        goto advance;
      /* also 0 is invalid, as it means get the info from the header and we
       * don't have headers if we are here */
      if (((c.data[3] >> 1) & 0x07) == 0x00)
        goto advance;

      /* next bit must be 0 */
      if ((c.data[3] & 0x01) == 0x01)
        goto advance;

      /* FIXME: shouldn't we include the crc check ? */

      GST_DEBUG ("Found flac without headers at %d", (gint) c.offset);
      gst_type_find_suggest (tf, GST_TYPE_FIND_POSSIBLE, FLAC_CAPS);
      return;
    }
  advance:
    data_scan_ctx_advance (tf, &c, 1);
  }
#endif
}
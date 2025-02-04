h265_video_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };

  /* Stream consists of: a series of sync codes (00 00 00 01) followed
   * by NALs
   */
  gboolean seen_irap = FALSE;
  gboolean seen_vps = FALSE;
  gboolean seen_sps = FALSE;
  gboolean seen_pps = FALSE;
  int nut;
  int good = 0;
  int bad = 0;

  while (c.offset < H265_MAX_PROBE_LENGTH) {
    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 5)))
      break;

    if (IS_MPEG_HEADER (c.data)) {
      /* forbiden_zero_bit | nal_unit_type */
      nut = c.data[3] & 0xfe;

      /* if forbidden bit is different to 0 won't be h265 */
      if (nut > 0x7e) {
        bad++;
        break;
      }
      nut = nut >> 1;

      /* if nuh_layer_id is not zero or nuh_temporal_id_plus1 is zero then
       * it won't be h265 */
      if ((c.data[3] & 0x01) || (c.data[4] & 0xf8) || !(c.data[4] & 0x07)) {
        bad++;
        break;
      }

      /* collect statistics about the NAL types */
      if ((nut >= 0 && nut <= 9) || (nut >= 16 && nut <= 21) || (nut >= 32
              && nut <= 40)) {
        if (nut == 32)
          seen_vps = TRUE;
        else if (nut == 33)
          seen_sps = TRUE;
        else if (nut == 34)
          seen_pps = TRUE;
        else if (nut >= 16 && nut <= 21) {
          /* BLA, IDR and CRA pictures are belongs to be IRAP picture */
          /* we are not counting the reserved IRAP pictures (22 and 23) to good */
          seen_irap = TRUE;
        }

        good++;
      } else if ((nut >= 10 && nut <= 15) || (nut >= 22 && nut <= 31)
          || (nut >= 41 && nut <= 47)) {
        /* reserved values are counting as bad */
        bad++;
      } else {
        /* unspecified (48..63), application specific */
        /* don't consider these as bad */
      }

      GST_LOG ("good:%d, bad:%d, pps:%d, sps:%d, vps:%d, irap:%d", good, bad,
          seen_pps, seen_sps, seen_vps, seen_irap);

      if (seen_sps && seen_pps && seen_irap && good >= 10 && bad < 4) {
        gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, H265_VIDEO_CAPS);
        return;
      }

      data_scan_ctx_advance (tf, &c, 5);
    }
    data_scan_ctx_advance (tf, &c, 1);
  }

  GST_LOG ("good:%d, bad:%d, pps:%d, sps:%d, vps:%d, irap:%d", good, bad,
      seen_pps, seen_sps, seen_vps, seen_irap);

  if (good >= 2 && bad == 0) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_POSSIBLE, H265_VIDEO_CAPS);
  }
}
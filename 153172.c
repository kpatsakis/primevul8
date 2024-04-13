mpeg4_video_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };
  gboolean seen_vios_at_0 = FALSE;
  gboolean seen_vios = FALSE;
  gboolean seen_vos = FALSE;
  gboolean seen_vol = FALSE;
  guint num_vop_headers = 0;
  guint8 sc;

  while (c.offset < GST_MPEGVID_TYPEFIND_TRY_SYNC) {
    if (num_vop_headers >= GST_MPEGVID_TYPEFIND_TRY_PICTURES)
      break;

    if (!mpeg_find_next_header (tf, &c,
            GST_MPEGVID_TYPEFIND_TRY_SYNC - c.offset))
      break;

    sc = c.data[0];

    /* visual_object_sequence_start_code */
    if (sc == 0xB0) {
      if (seen_vios)
        break;                  /* Terminate at second vios */
      if (c.offset == 0)
        seen_vios_at_0 = TRUE;
      seen_vios = TRUE;
      data_scan_ctx_advance (tf, &c, 2);
      if (!mpeg_find_next_header (tf, &c, 0))
        break;

      sc = c.data[0];

      /* Optional metadata */
      if (sc == 0xB2)
        if (!mpeg_find_next_header (tf, &c, 24))
          break;
    }

    /* visual_object_start_code (consider it optional) */
    if (sc == 0xB5) {
      data_scan_ctx_advance (tf, &c, 2);
      /* may contain ID marker and YUV clamping */
      if (!mpeg_find_next_header (tf, &c, 7))
        break;

      sc = c.data[0];
    }

    /* video_object_start_code */
    if (sc <= 0x1F) {
      if (seen_vos)
        break;                  /* Terminate at second vos */
      seen_vos = TRUE;
      data_scan_ctx_advance (tf, &c, 2);
      continue;
    }

    /* video_object_layer_start_code */
    if (sc >= 0x20 && sc <= 0x2F) {
      seen_vol = TRUE;
      data_scan_ctx_advance (tf, &c, 5);
      continue;
    }

    /* video_object_plane_start_code */
    if (sc == 0xB6) {
      num_vop_headers++;
      data_scan_ctx_advance (tf, &c, 2);
      continue;
    }

    /* Unknown start code. */
  }

  if (num_vop_headers > 0 || seen_vol) {
    GstTypeFindProbability probability = 0;

    GST_LOG ("Found %d pictures, vios: %d, vos:%d, vol:%d", num_vop_headers,
        seen_vios, seen_vos, seen_vol);

    if (num_vop_headers >= GST_MPEGVID_TYPEFIND_TRY_PICTURES && seen_vios_at_0
        && seen_vos && seen_vol)
      probability = GST_TYPE_FIND_MAXIMUM - 1;
    else if (num_vop_headers >= GST_MPEGVID_TYPEFIND_TRY_PICTURES && seen_vios
        && seen_vos && seen_vol)
      probability = GST_TYPE_FIND_NEARLY_CERTAIN - 1;
    else if (seen_vios_at_0 && seen_vos && seen_vol)
      probability = GST_TYPE_FIND_NEARLY_CERTAIN - 6;
    else if (num_vop_headers >= GST_MPEGVID_TYPEFIND_TRY_PICTURES && seen_vos
        && seen_vol)
      probability = GST_TYPE_FIND_NEARLY_CERTAIN - 6;
    else if (num_vop_headers >= GST_MPEGVID_TYPEFIND_TRY_PICTURES && seen_vol)
      probability = GST_TYPE_FIND_NEARLY_CERTAIN - 9;
    else if (num_vop_headers >= GST_MPEGVID_TYPEFIND_TRY_PICTURES)
      probability = GST_TYPE_FIND_LIKELY - 1;
    else if (num_vop_headers > 2 && seen_vios && seen_vos && seen_vol)
      probability = GST_TYPE_FIND_LIKELY - 9;
    else if (seen_vios && seen_vos && seen_vol)
      probability = GST_TYPE_FIND_LIKELY - 20;
    else if (num_vop_headers > 0 && seen_vos && seen_vol)
      probability = GST_TYPE_FIND_POSSIBLE;
    else if (num_vop_headers > 0)
      probability = GST_TYPE_FIND_POSSIBLE - 10;
    else if (seen_vos && seen_vol)
      probability = GST_TYPE_FIND_POSSIBLE - 20;

    gst_type_find_suggest (tf, probability, MPEG4_VIDEO_CAPS);
  }
}
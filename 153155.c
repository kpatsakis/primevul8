mpeg_video_stream_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };
  gboolean seen_seq_at_0 = FALSE;
  gboolean seen_seq = FALSE;
  gboolean seen_gop = FALSE;
  guint64 last_pic_offset = 0;
  guint num_pic_headers = 0;
  gint found = 0;

  while (c.offset < GST_MPEGVID_TYPEFIND_TRY_SYNC) {
    if (found >= GST_MPEGVID_TYPEFIND_TRY_PICTURES)
      break;

    if (!data_scan_ctx_ensure_data (tf, &c, 5))
      break;

    if (!IS_MPEG_HEADER (c.data))
      goto next;

    /* a pack header indicates that this isn't an elementary stream */
    if (c.data[3] == 0xBA && mpeg_sys_is_valid_pack (tf, c.data, c.size, NULL))
      return;

    /* do we have a sequence header? */
    if (c.data[3] == 0xB3) {
      seen_seq_at_0 = seen_seq_at_0 || (c.offset == 0);
      seen_seq = TRUE;
      data_scan_ctx_advance (tf, &c, 4 + 8);
      continue;
    }

    /* or a GOP header */
    if (c.data[3] == 0xB8) {
      seen_gop = TRUE;
      data_scan_ctx_advance (tf, &c, 8);
      continue;
    }

    /* but what we'd really like to see is a picture header */
    if (c.data[3] == 0x00) {
      ++num_pic_headers;
      last_pic_offset = c.offset;
      data_scan_ctx_advance (tf, &c, 8);
      continue;
    }

    /* ... each followed by a slice header with slice_vertical_pos=1 that's
     * not too far away from the previously seen picture header. */
    if (c.data[3] == 0x01 && num_pic_headers > found &&
        (c.offset - last_pic_offset) >= 4 &&
        (c.offset - last_pic_offset) <= 64) {
      data_scan_ctx_advance (tf, &c, 4);
      found += 1;
      continue;
    }

  next:

    data_scan_ctx_advance (tf, &c, 1);
  }

  if (found > 0 || seen_seq) {
    GstTypeFindProbability probability = 0;

    GST_LOG ("Found %d pictures, seq:%d, gop:%d", found, seen_seq, seen_gop);

    if (found >= GST_MPEGVID_TYPEFIND_TRY_PICTURES && seen_seq && seen_gop)
      probability = GST_TYPE_FIND_NEARLY_CERTAIN - 1;
    else if (found >= GST_MPEGVID_TYPEFIND_TRY_PICTURES && seen_seq)
      probability = GST_TYPE_FIND_NEARLY_CERTAIN - 9;
    else if (found >= GST_MPEGVID_TYPEFIND_TRY_PICTURES)
      probability = GST_TYPE_FIND_LIKELY;
    else if (seen_seq_at_0 && seen_gop && found > 2)
      probability = GST_TYPE_FIND_LIKELY - 10;
    else if (seen_seq && seen_gop && found > 2)
      probability = GST_TYPE_FIND_LIKELY - 20;
    else if (seen_seq_at_0 && found > 0)
      probability = GST_TYPE_FIND_POSSIBLE;
    else if (seen_seq && found > 0)
      probability = GST_TYPE_FIND_POSSIBLE - 5;
    else if (found > 0)
      probability = GST_TYPE_FIND_POSSIBLE - 10;
    else if (seen_seq)
      probability = GST_TYPE_FIND_POSSIBLE - 20;

    gst_type_find_suggest_simple (tf, probability, "video/mpeg",
        "systemstream", G_TYPE_BOOLEAN, FALSE,
        "mpegversion", G_TYPE_INT, 1, "parsed", G_TYPE_BOOLEAN, FALSE, NULL);
  }
}
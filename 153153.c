mp3_type_find (GstTypeFind * tf, gpointer unused)
{
  GstTypeFindProbability prob, mid_prob;
  const guint8 *data;
  guint layer, mid_layer;
  guint64 length;

  mp3_type_find_at_offset (tf, 0, &layer, &prob);
  length = gst_type_find_get_length (tf);

  if (length == 0 || length == (guint64) - 1) {
    if (prob != 0)
      goto suggest;
    return;
  }

  /* if we're pretty certain already, skip the additional check */
  if (prob >= GST_TYPE_FIND_LIKELY)
    goto suggest;

  mp3_type_find_at_offset (tf, length / 2, &mid_layer, &mid_prob);

  if (mid_prob > 0) {
    if (prob == 0) {
      GST_LOG ("detected audio/mpeg only in the middle (p=%u)", mid_prob);
      layer = mid_layer;
      prob = mid_prob;
      goto suggest;
    }

    if (layer != mid_layer) {
      GST_WARNING ("audio/mpeg layer discrepancy: %u vs. %u", layer, mid_layer);
      return;                   /* FIXME: or should we just go with the one in the middle? */
    }

    /* detected mpeg audio both in middle of the file and at the start */
    prob = (prob + mid_prob) / 2;
    goto suggest;
  }

  /* a valid header right at the start makes it more likely
   * that this is actually plain mpeg-1 audio */
  if (prob > 0) {
    data = gst_type_find_peek (tf, 0, 4);       /* use min. frame size? */
    if (data && mp3_type_frame_length_from_header (GST_READ_UINT32_BE (data),
            &layer, NULL, NULL, NULL, NULL, 0) != 0) {
      prob = MIN (prob + 10, GST_TYPE_FIND_MAXIMUM);
    }
  }

  if (prob > 0)
    goto suggest;

  return;

suggest:
  {
    g_return_if_fail (layer >= 1 && layer <= 3);

    gst_type_find_suggest_simple (tf, prob, "audio/mpeg",
        "mpegversion", G_TYPE_INT, 1, "layer", G_TYPE_INT, layer,
        "parsed", G_TYPE_BOOLEAN, FALSE, NULL);
  }
}
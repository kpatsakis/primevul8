gst_riff_wavext_add_channel_mask (GstCaps * caps, gint num_channels,
    guint32 layout, gint channel_reorder_map[18])
{
  gint i, p;
  guint64 channel_mask = 0;
  GstAudioChannelPosition *from, *to;
  gboolean ret = FALSE;

  if (num_channels < 1) {
    GST_DEBUG ("invalid number of channels: %d", num_channels);
    return FALSE;
  }

  from = g_new (GstAudioChannelPosition, num_channels);
  to = g_new (GstAudioChannelPosition, num_channels);
  p = 0;
  for (i = 0; i < MAX_CHANNEL_POSITIONS; ++i) {
    if ((layout & layout_mapping[i].ms_mask) != 0) {
      if (p >= num_channels) {
        GST_WARNING ("More bits set in the channel layout map than there "
            "are channels! Setting channel-mask to 0.");
        channel_mask = 0;
        break;
      }
      channel_mask |= G_GUINT64_CONSTANT (1) << layout_mapping[i].gst_pos;
      from[p] = layout_mapping[i].gst_pos;
      ++p;
    }
  }

  if (channel_mask > 0 && channel_reorder_map) {
    if (p != num_channels) {
      /* WAVEFORMATEXTENSIBLE allows to have more channels than bits in
       * the channel mask. We accept this, too, and hope that downstream
       * can handle this */
      GST_WARNING ("Partially unknown positions in channel mask");
      for (; p < num_channels; ++p)
        from[p] = GST_AUDIO_CHANNEL_POSITION_INVALID;
    }
    memcpy (to, from, sizeof (from[0]) * num_channels);
    if (!gst_audio_channel_positions_to_valid_order (to, num_channels))
      goto fail;
    if (!gst_audio_get_channel_reorder_map (num_channels, from, to,
            channel_reorder_map))
      goto fail;
  }

  gst_caps_set_simple (caps, "channel-mask", GST_TYPE_BITMASK, channel_mask,
      NULL);

  ret = TRUE;

fail:
  g_free (from);
  g_free (to);

  return ret;
}
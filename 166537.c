gst_riff_wavext_get_default_channel_mask (guint nchannels)
{
  guint32 channel_mask = 0;

  /* Set the default channel mask for the given number of channels.
   * http://www.microsoft.com/whdc/device/audio/multichaud.mspx
   */
  switch (nchannels) {
    case 11:
      channel_mask |= 0x00400;
      channel_mask |= 0x00200;
    case 9:
      channel_mask |= 0x00100;
    case 8:
      channel_mask |= 0x00080;
      channel_mask |= 0x00040;
    case 6:
      channel_mask |= 0x00020;
      channel_mask |= 0x00010;
    case 4:
      channel_mask |= 0x00008;
    case 3:
      channel_mask |= 0x00004;
    case 2:
      channel_mask |= 0x00002;
      channel_mask |= 0x00001;
      break;
  }

  return channel_mask;
}
flx_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 134);

  if (data) {
    /* check magic and the frame type of the first frame */
    if ((data[4] == 0x11 || data[4] == 0x12 ||
            data[4] == 0x30 || data[4] == 0x44) &&
        data[5] == 0xaf &&
        ((data[132] == 0x00 || data[132] == 0xfa) && data[133] == 0xf1)) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, FLX_CAPS);
    }
    return;
  }
  data = gst_type_find_peek (tf, 0, 6);
  if (data) {
    /* check magic only */
    if ((data[4] == 0x11 || data[4] == 0x12 ||
            data[4] == 0x30 || data[4] == 0x44) && data[5] == 0xaf) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, FLX_CAPS);
    }
    return;
  }
}
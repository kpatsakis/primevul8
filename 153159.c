exr_type_find (GstTypeFind * tf, gpointer ununsed)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 8);

  if (data) {
    guint32 flags;

    if (GST_READ_UINT32_LE (data) != 0x01312f76)
      return;

    flags = GST_READ_UINT32_LE (data + 4);
    if ((flags & 0xff) != 1 && (flags & 0xff) != 2)
      return;

    /* If bit 9 is set, bit 11 and 12 must be 0 */
    if ((flags & 0x200) && (flags & 0x1800))
      return;

    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, EXR_CAPS);
  }
}
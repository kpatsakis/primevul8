aa_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 12);
  if (data == NULL)
    return;

  if (GST_READ_UINT32_BE (data + 4) == 0x57907536) {
    guint64 media_len;

    media_len = gst_type_find_get_length (tf);
    if (media_len > 0 && GST_READ_UINT32_BE (data) == media_len)
      gst_type_find_suggest (tf, GST_TYPE_FIND_NEARLY_CERTAIN, AA_CAPS);
    else
      gst_type_find_suggest (tf, GST_TYPE_FIND_POSSIBLE, AA_CAPS);
  }
}
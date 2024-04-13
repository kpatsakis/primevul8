dvdiso_type_find (GstTypeFind * tf, gpointer private)
{
  /* 0x8000 bytes of zeros, then "\001CD001" */
  gint64 len;
  const guint8 *data;

  len = gst_type_find_get_length (tf);
  if (len < 0x8006)
    return;
  data = gst_type_find_peek (tf, 0, 0x8006);
  if (G_UNLIKELY (data == NULL))
    return;
  for (len = 0; len < 0x8000; len++)
    if (data[len])
      return;
  /* Can the '1' be anything else ? My three samples all have '1'. */
  if (memcmp (data + 0x8000, "\001CD001", 6))
    return;

  /* May need more inspection, we may be able to demux some of them */
  gst_type_find_suggest_simple (tf, GST_TYPE_FIND_LIKELY,
      "application/octet-stream", NULL);
}
jp2_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 24);
  if (!data)
    return;

  /* jp2 signature */
  if (memcmp (data, "\000\000\000\014jP  \015\012\207\012", 12) != 0)
    return;

  /* check ftyp box */
  data += 12;
  if (memcmp (data + 4, "ftyp", 4) == 0) {
    if (memcmp (data + 8, "jp2 ", 4) == 0)
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, JP2_CAPS);
    else if (memcmp (data + 8, "mjp2", 4) == 0)
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MJ2_CAPS);
  }
}
oggskel_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 12);

  if (data) {
    /* 8 byte string "fishead\0" for the ogg skeleton stream */
    if (memcmp (data, "fishead\0", 8) != 0)
      return;
    data += 8;

    /* Require that the header contains version 3.0 */
    if (GST_READ_UINT16_LE (data) != 3)
      return;
    data += 2;
    if (GST_READ_UINT16_LE (data) != 0)
      return;

    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, OGG_SKELETON_CAPS);
  }
}
theora_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 7);   //42);

  if (data) {
    if (data[0] != 0x80)
      return;
    if (memcmp (&data[1], "theora", 6) != 0)
      return;
    /* FIXME: make this more reliable when specs are out */

    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, THEORA_CAPS);
  }
}
swf_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 4);

  if (data && (data[0] == 'F' || data[0] == 'C') &&
      data[1] == 'W' && data[2] == 'S') {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, SWF_CAPS);
  }
}
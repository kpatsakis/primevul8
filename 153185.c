mid_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 4);

  /* http://jedi.ks.uiuc.edu/~johns/links/music/midifile.html */
  if (data && data[0] == 'M' && data[1] == 'T' && data[2] == 'h'
      && data[3] == 'd')
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MID_CAPS);
}
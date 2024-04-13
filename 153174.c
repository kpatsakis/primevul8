mxmf_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = NULL;

  /* Search FileId "XMF_" 4 bytes */
  data = gst_type_find_peek (tf, 0, 4);
  if (data && data[0] == 'X' && data[1] == 'M' && data[2] == 'F'
      && data[3] == '_') {
    /* Search Format version "2.00" 4 bytes */
    data = gst_type_find_peek (tf, 4, 4);
    if (data && data[0] == '2' && data[1] == '.' && data[2] == '0'
        && data[3] == '0') {
      /* Search TypeId 2     1 byte */
      data = gst_type_find_peek (tf, 11, 1);
      if (data && data[0] == 2) {
        gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MXMF_CAPS);
      }
    }
  }
}
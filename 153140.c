cmml_type_find (GstTypeFind * tf, gpointer private)
{
  /* Header is 12 bytes minimum (though we don't check the minor version */
  const guint8 *data = gst_type_find_peek (tf, 0, 12);

  if (data) {

    /* 8 byte string "CMML\0\0\0\0" for the magic number */
    if (memcmp (data, "CMML\0\0\0\0", 8) != 0)
      return;
    data += 8;

    /* Require that the header contains at least version 2.0 */
    if (GST_READ_UINT16_LE (data) < 2)
      return;

    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, CMML_CAPS);
  }
}
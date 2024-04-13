wbmp_typefind (GstTypeFind * find, gpointer user_data)
{
  const guint8 *data;
  gint64 datalen;
  guint w, h, size;

  /* http://en.wikipedia.org/wiki/Wireless_Application_Protocol_Bitmap_Format */
  datalen = gst_type_find_get_length (find);
  if (datalen == 0)
    return;

  data = gst_type_find_peek (find, 0, 5);
  if (data == NULL)
    return;

  /* want 0x00 0x00 at start */
  if (*data++ != 0 || *data++ != 0)
    return;

  /* min header size */
  size = 4;

  /* let's assume max width/height is 65536 */
  w = *data++;
  if ((w & 0x80)) {
    w = (w << 8) | *data++;
    if ((w & 0x80))
      return;
    ++size;
    data = gst_type_find_peek (find, 4, 2);
    if (data == NULL)
      return;
  }
  h = *data++;
  if ((h & 0x80)) {
    h = (h << 8) | *data++;
    if ((h & 0x80))
      return;
    ++size;
  }

  if (w == 0 || h == 0)
    return;

  /* now add bitmap size */
  size += h * (GST_ROUND_UP_8 (w) / 8);

  if (datalen == size) {
    gst_type_find_suggest_simple (find, GST_TYPE_FIND_POSSIBLE - 10,
        "image/vnd.wap.wbmp", NULL);
  }
}
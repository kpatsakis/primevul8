postscript_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, 3);
  if (!data)
    return;

  if (data[0] == 0x04)
    data++;
  if (data[0] == '%' && data[1] == '!')
    gst_type_find_suggest (tf, GST_TYPE_FIND_POSSIBLE, POSTSCRIPT_CAPS);

}
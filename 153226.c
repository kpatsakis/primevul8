pva_type_find (GstTypeFind * tf, gpointer private)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 5);

  if (data == NULL)
    return;

  if (data[0] == 'A' && data[1] == 'V' && data[2] < 3 && data[4] == 0x55)
    gst_type_find_suggest (tf, GST_TYPE_FIND_NEARLY_CERTAIN, PVA_CAPS);
}
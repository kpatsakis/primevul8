riff_type_find (GstTypeFind * tf, gpointer private)
{
  GstTypeFindData *riff_data = (GstTypeFindData *) private;
  const guint8 *data = gst_type_find_peek (tf, 0, 12);

  if (data && (memcmp (data, "RIFF", 4) == 0 || memcmp (data, "AVF0", 4) == 0)) {
    data += 8;
    if (memcmp (data, riff_data->data, 4) == 0)
      gst_type_find_suggest (tf, riff_data->probability, riff_data->caps);
  }
}
start_with_type_find (GstTypeFind * tf, gpointer private)
{
  GstTypeFindData *start_with = (GstTypeFindData *) private;
  const guint8 *data;

  GST_LOG ("trying to find mime type %s with the first %u bytes of data",
      gst_structure_get_name (gst_caps_get_structure (start_with->caps, 0)),
      start_with->size);
  data = gst_type_find_peek (tf, 0, start_with->size);
  if (data && memcmp (data, start_with->data, start_with->size) == 0) {
    gst_type_find_suggest (tf, start_with->probability, start_with->caps);
  }
}
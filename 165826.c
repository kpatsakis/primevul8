_parse_cat (GstMpegtsSection * section)
{
  guint8 *data;
  guint desc_len;

  /* Skip parts already parsed */
  data = section->data + 8;

  /* descriptors */
  desc_len = section->section_length - 4 - 8;
  return (gpointer) gst_mpegts_parse_descriptors (data, desc_len);
}
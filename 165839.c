_gst_mpegts_section_free (GstMpegtsSection * section)
{
  GST_DEBUG ("Freeing section type %d", section->section_type);

  if (section->cached_parsed && section->destroy_parsed)
    section->destroy_parsed (section->cached_parsed);

  g_free (section->data);

  g_slice_free (GstMpegtsSection, section);
}
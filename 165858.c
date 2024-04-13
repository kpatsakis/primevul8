gst_mpegts_section_get_tsdt (GstMpegtsSection * section)
{
  g_return_val_if_fail (section->section_type == GST_MPEGTS_SECTION_TSDT, NULL);
  g_return_val_if_fail (section->cached_parsed || section->data, NULL);

  if (section->cached_parsed)
    return g_ptr_array_ref ((GPtrArray *) section->cached_parsed);

  /* FIXME : parse TSDT */
  return NULL;
}
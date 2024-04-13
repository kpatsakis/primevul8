gst_mpegts_section_get_pat (GstMpegtsSection * section)
{
  g_return_val_if_fail (section->section_type == GST_MPEGTS_SECTION_PAT, NULL);
  g_return_val_if_fail (section->cached_parsed || section->data, NULL);

  if (!section->cached_parsed)
    section->cached_parsed =
        __common_section_checks (section, 12, _parse_pat,
        (GDestroyNotify) g_ptr_array_unref);

  if (section->cached_parsed)
    return g_ptr_array_ref ((GPtrArray *) section->cached_parsed);
  return NULL;
}
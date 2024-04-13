gst_mpegts_section_get_pmt (GstMpegtsSection * section)
{
  g_return_val_if_fail (section->section_type == GST_MPEGTS_SECTION_PMT, NULL);
  g_return_val_if_fail (section->cached_parsed || section->data, NULL);

  if (!section->cached_parsed)
    section->cached_parsed =
        __common_section_checks (section, 16, _parse_pmt,
        (GDestroyNotify) _gst_mpegts_pmt_free);

  return (const GstMpegtsPMT *) section->cached_parsed;
}
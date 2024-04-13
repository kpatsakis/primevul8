gst_mpegts_section_from_pmt (GstMpegtsPMT * pmt, guint16 pid)
{
  GstMpegtsSection *section;

  g_return_val_if_fail (pmt != NULL, NULL);

  section = _gst_mpegts_section_init (pid, GST_MTS_TABLE_ID_TS_PROGRAM_MAP);

  section->subtable_extension = pmt->program_number;
  section->cached_parsed = (gpointer) pmt;
  section->packetizer = _packetize_pmt;
  section->destroy_parsed = (GDestroyNotify) _gst_mpegts_pmt_free;

  return section;
}
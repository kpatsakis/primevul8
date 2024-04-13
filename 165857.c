gst_mpegts_section_from_pat (GPtrArray * programs, guint16 ts_id)
{
  GstMpegtsSection *section;

  section = _gst_mpegts_section_init (0x00,
      GST_MTS_TABLE_ID_PROGRAM_ASSOCIATION);

  section->subtable_extension = ts_id;
  section->cached_parsed = (gpointer) programs;
  section->packetizer = _packetize_pat;
  section->destroy_parsed = (GDestroyNotify) g_ptr_array_unref;

  return section;
}
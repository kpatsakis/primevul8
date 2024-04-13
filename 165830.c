_gst_mpegts_section_init (guint16 pid, guint8 table_id)
{
  GstMpegtsSection *section;

  section = g_slice_new0 (GstMpegtsSection);
  gst_mini_object_init (GST_MINI_OBJECT_CAST (section), 0, MPEG_TYPE_TS_SECTION,
      (GstMiniObjectCopyFunction) _gst_mpegts_section_copy, NULL,
      (GstMiniObjectFreeFunction) _gst_mpegts_section_free);

  section->pid = pid;
  section->table_id = table_id;
  section->current_next_indicator = TRUE;
  section->section_type = _identify_section (pid, table_id);

  return section;
}
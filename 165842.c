_gst_mpegts_section_copy (GstMpegtsSection * section)
{
  GstMpegtsSection *copy;

  copy = g_slice_new0 (GstMpegtsSection);
  gst_mini_object_init (GST_MINI_OBJECT_CAST (copy), 0, MPEG_TYPE_TS_SECTION,
      (GstMiniObjectCopyFunction) _gst_mpegts_section_copy, NULL,
      (GstMiniObjectFreeFunction) _gst_mpegts_section_free);

  copy->section_type = section->section_type;
  copy->pid = section->pid;
  copy->table_id = section->table_id;
  copy->subtable_extension = section->subtable_extension;
  copy->version_number = section->version_number;
  copy->current_next_indicator = section->current_next_indicator;
  copy->section_number = section->section_number;
  copy->last_section_number = section->last_section_number;
  copy->crc = section->crc;

  copy->data = g_memdup (section->data, section->section_length);
  copy->section_length = section->section_length;
  /* Note: We do not copy the cached parsed item, it will be
   * reconstructed on that copy */
  copy->cached_parsed = NULL;
  copy->offset = section->offset;
  copy->short_section = section->short_section;

  return copy;
}
gst_event_parse_mpegts_section (GstEvent * event)
{
  const GstStructure *structure;
  GstMpegtsSection *section;

  structure = gst_event_get_structure (event);

  if (!gst_structure_id_get (structure, QUARK_SECTION, MPEG_TYPE_TS_SECTION,
          &section, NULL))
    return NULL;

  return section;
}
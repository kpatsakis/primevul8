_mpegts_section_get_structure (GstMpegtsSection * section)
{
  GstStructure *st;
  GQuark quark;

  switch (section->section_type) {
    case GST_MPEGTS_SECTION_PAT:
      quark = QUARK_PAT;
      break;
    case GST_MPEGTS_SECTION_PMT:
      quark = QUARK_PMT;
      break;
    case GST_MPEGTS_SECTION_CAT:
      quark = QUARK_CAT;
      break;
    case GST_MPEGTS_SECTION_EIT:
      quark = QUARK_EIT;
      break;
    case GST_MPEGTS_SECTION_BAT:
      quark = QUARK_BAT;
      break;
    case GST_MPEGTS_SECTION_NIT:
      quark = QUARK_NIT;
      break;
    case GST_MPEGTS_SECTION_SDT:
      quark = QUARK_SDT;
      break;
    case GST_MPEGTS_SECTION_TDT:
      quark = QUARK_TDT;
      break;
    case GST_MPEGTS_SECTION_TOT:
      quark = QUARK_TOT;
      break;
    default:
      GST_DEBUG ("Creating structure for unknown GstMpegtsSection");
      quark = QUARK_SECTION;
      break;
  }

  st = gst_structure_new_id (quark, QUARK_SECTION, MPEG_TYPE_TS_SECTION,
      section, NULL);

  return st;
}
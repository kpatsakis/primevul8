gst_mpegts_initialize (void)
{
  if (_gst_mpegts_section_type)
    return;

  GST_DEBUG_CATEGORY_INIT (gst_mpegts_debug, "mpegts", 0,
      "MPEG-TS helper library");

  /* FIXME : Temporary hack to initialize section gtype */
  _gst_mpegts_section_type = gst_mpegts_section_get_type ();

  QUARK_PAT = g_quark_from_string ("pat");
  QUARK_CAT = g_quark_from_string ("cat");
  QUARK_PMT = g_quark_from_string ("pmt");
  QUARK_NIT = g_quark_from_string ("nit");
  QUARK_BAT = g_quark_from_string ("bat");
  QUARK_SDT = g_quark_from_string ("sdt");
  QUARK_EIT = g_quark_from_string ("eit");
  QUARK_TDT = g_quark_from_string ("tdt");
  QUARK_TOT = g_quark_from_string ("tot");
  QUARK_SECTION = g_quark_from_string ("section");

  __initialize_descriptors ();
}
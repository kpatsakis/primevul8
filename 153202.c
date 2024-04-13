svg_type_find (GstTypeFind * tf, gpointer unused)
{
  static const gchar svg_doctype[] = "!DOCTYPE svg";
  static const gchar svg_tag[] = "<svg";
  DataScanCtx c = { 0, NULL, 0 };

  while (c.offset <= 1024) {
    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 12)))
      break;

    if (memcmp (svg_doctype, c.data, 12) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, SVG_CAPS);
      return;
    } else if (memcmp (svg_tag, c.data, 4) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, SVG_CAPS);
      return;
    }
    data_scan_ctx_advance (tf, &c, 1);
  }
}
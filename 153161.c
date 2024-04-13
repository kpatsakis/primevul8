pnm_type_find (GstTypeFind * tf, gpointer ununsed)
{
  const gchar *media_type = NULL;
  DataScanCtx c = { 0, NULL, 0 };
  guint h = 0, w = 0;

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 16)))
    return;

  /* see http://en.wikipedia.org/wiki/Netpbm_format */
  if (c.data[0] != 'P' || c.data[1] < '1' || c.data[1] > '7' ||
      !IS_PNM_WHITESPACE (c.data[2]) ||
      (c.data[3] != '#' && c.data[3] < '0' && c.data[3] > '9'))
    return;

  switch (c.data[1]) {
    case '1':
      media_type = "image/x-portable-bitmap";   /* ASCII */
      break;
    case '2':
      media_type = "image/x-portable-graymap";  /* ASCII */
      break;
    case '3':
      media_type = "image/x-portable-pixmap";   /* ASCII */
      break;
    case '4':
      media_type = "image/x-portable-bitmap";   /* Raw */
      break;
    case '5':
      media_type = "image/x-portable-graymap";  /* Raw */
      break;
    case '6':
      media_type = "image/x-portable-pixmap";   /* Raw */
      break;
    case '7':
      media_type = "image/x-portable-anymap";
      break;
    default:
      g_return_if_reached ();
  }

  /* try to extract width and height as well */
  if (c.data[1] != '7') {
    gchar s[64] = { 0, }
    , sep1, sep2;

    /* need to skip any comment lines first */
    data_scan_ctx_advance (tf, &c, 3);
    while (c.data[0] == '#') {  /* we know there's still data left */
      data_scan_ctx_advance (tf, &c, 1);
      while (c.data[0] != '\n' && c.data[0] != '\r') {
        if (!data_scan_ctx_ensure_data (tf, &c, 4))
          return;
        data_scan_ctx_advance (tf, &c, 1);
      }
      data_scan_ctx_advance (tf, &c, 1);
      GST_LOG ("skipped comment line in PNM header");
    }

    if (!data_scan_ctx_ensure_data (tf, &c, 32) &&
        !data_scan_ctx_ensure_data (tf, &c, 4)) {
      return;
    }

    /* need to NUL-terminate data for sscanf */
    memcpy (s, c.data, MIN (sizeof (s) - 1, c.size));
    if (sscanf (s, "%u%c%u%c", &w, &sep1, &h, &sep2) == 4 &&
        IS_PNM_WHITESPACE (sep1) && IS_PNM_WHITESPACE (sep2) &&
        w > 0 && w < G_MAXINT && h > 0 && h < G_MAXINT) {
      GST_LOG ("extracted PNM width and height: %dx%d", w, h);
    } else {
      w = 0;
      h = 0;
    }
  } else {
    /* FIXME: extract width + height for anymaps too */
  }

  if (w > 0 && h > 0) {
    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM, media_type,
        "width", G_TYPE_INT, w, "height", G_TYPE_INT, h, NULL);
  } else {
    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_LIKELY, media_type, NULL);
  }
}
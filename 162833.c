parse_protocol_version (gchar * protocol, GstRTSPMsgType * type,
    GstRTSPVersion * version)
{
  GstRTSPVersion rversion;
  GstRTSPResult res = GST_RTSP_OK;
  gchar *ver;

  if (G_LIKELY ((ver = strchr (protocol, '/')) != NULL)) {
    guint major;
    guint minor;
    gchar dummychar;

    *ver++ = '\0';

    /* the version number must be formatted as X.Y with nothing following */
    if (sscanf (ver, "%u.%u%c", &major, &minor, &dummychar) != 2)
      res = GST_RTSP_EPARSE;

    rversion = major * 0x10 + minor;
    if (g_ascii_strcasecmp (protocol, "RTSP") == 0) {

      if (rversion != GST_RTSP_VERSION_1_0 && rversion != GST_RTSP_VERSION_2_0) {
        *version = GST_RTSP_VERSION_INVALID;
        res = GST_RTSP_ERROR;
      }
    } else if (g_ascii_strcasecmp (protocol, "HTTP") == 0) {
      if (*type == GST_RTSP_MESSAGE_REQUEST)
        *type = GST_RTSP_MESSAGE_HTTP_REQUEST;
      else if (*type == GST_RTSP_MESSAGE_RESPONSE)
        *type = GST_RTSP_MESSAGE_HTTP_RESPONSE;

      if (rversion != GST_RTSP_VERSION_1_0 &&
          rversion != GST_RTSP_VERSION_1_1 && rversion != GST_RTSP_VERSION_2_0)
        res = GST_RTSP_ERROR;
    } else
      res = GST_RTSP_EPARSE;
  } else
    res = GST_RTSP_EPARSE;

  if (res == GST_RTSP_OK)
    *version = rversion;

  return res;
}
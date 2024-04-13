parse_response_status (guint8 * buffer, GstRTSPMessage * msg)
{
  GstRTSPResult res = GST_RTSP_OK;
  GstRTSPResult res2;
  gchar versionstr[20];
  gchar codestr[4];
  gint code;
  gchar *bptr;

  bptr = (gchar *) buffer;

  if (parse_string (versionstr, sizeof (versionstr), &bptr) != GST_RTSP_OK)
    res = GST_RTSP_EPARSE;

  if (parse_string (codestr, sizeof (codestr), &bptr) != GST_RTSP_OK)
    res = GST_RTSP_EPARSE;
  code = atoi (codestr);
  if (G_UNLIKELY (*codestr == '\0' || code < 0 || code >= 600))
    res = GST_RTSP_EPARSE;

  while (g_ascii_isspace (*bptr))
    bptr++;

  if (G_UNLIKELY (gst_rtsp_message_init_response (msg, code, bptr,
              NULL) != GST_RTSP_OK))
    res = GST_RTSP_EPARSE;

  res2 = parse_protocol_version (versionstr, &msg->type,
      &msg->type_data.response.version);
  if (G_LIKELY (res == GST_RTSP_OK))
    res = res2;

  return res;
}
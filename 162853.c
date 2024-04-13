gst_rtsp_connection_set_http_mode (GstRTSPConnection * conn, gboolean enable)
{
  g_return_if_fail (conn != NULL);

  conn->manual_http = enable;
}
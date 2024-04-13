gst_rtsp_connection_set_ip (GstRTSPConnection * conn, const gchar * ip)
{
  g_return_if_fail (conn != NULL);

  g_free (conn->remote_ip);
  conn->remote_ip = g_strdup (ip);
}
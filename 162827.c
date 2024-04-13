gst_rtsp_connection_set_proxy (GstRTSPConnection * conn,
    const gchar * host, guint port)
{
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  g_free (conn->proxy_host);
  conn->proxy_host = g_strdup (host);
  conn->proxy_port = port;

  return GST_RTSP_OK;
}
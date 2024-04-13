gst_rtsp_connection_get_url (const GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, NULL);

  return conn->url;
}
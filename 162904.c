gst_rtsp_connection_set_tunneled (GstRTSPConnection * conn, gboolean tunneled)
{
  g_return_if_fail (conn != NULL);
  g_return_if_fail (conn->read_socket == NULL);
  g_return_if_fail (conn->write_socket == NULL);

  conn->tunneled = tunneled;
}
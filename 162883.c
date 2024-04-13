gst_rtsp_connection_get_read_socket (const GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, NULL);
  g_return_val_if_fail (conn->read_socket != NULL, NULL);

  return conn->read_socket;
}
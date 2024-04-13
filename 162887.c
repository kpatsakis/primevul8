gst_rtsp_connection_get_write_socket (const GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, NULL);
  g_return_val_if_fail (conn->write_socket != NULL, NULL);

  return conn->write_socket;
}
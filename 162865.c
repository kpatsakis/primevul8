gst_rtsp_connection_get_tls_validation_flags (GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, 0);

  return g_socket_client_get_tls_validation_flags (conn->client);
}
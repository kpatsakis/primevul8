gst_rtsp_connection_set_tls_validation_flags (GstRTSPConnection * conn,
    GTlsCertificateFlags flags)
{
  gboolean res = FALSE;

  g_return_val_if_fail (conn != NULL, FALSE);

  res = g_socket_client_get_tls (conn->client);
  if (res)
    g_socket_client_set_tls_validation_flags (conn->client, flags);

  return res;
}
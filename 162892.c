gst_rtsp_connection_accept (GSocket * socket, GstRTSPConnection ** conn,
    GCancellable * cancellable)
{
  GError *err = NULL;
  gchar *ip;
  guint16 port;
  GSocket *client_sock;
  GstRTSPResult ret;

  g_return_val_if_fail (G_IS_SOCKET (socket), GST_RTSP_EINVAL);
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  client_sock = g_socket_accept (socket, cancellable, &err);
  if (!client_sock)
    goto accept_failed;

  /* get the remote ip address and port */
  if (!collect_addresses (client_sock, &ip, &port, TRUE, &err))
    goto getnameinfo_failed;

  ret =
      gst_rtsp_connection_create_from_socket (client_sock, ip, port, NULL,
      conn);
  g_object_unref (client_sock);
  g_free (ip);

  return ret;

  /* ERRORS */
accept_failed:
  {
    GST_DEBUG ("Accepting client failed: %s", err->message);
    g_clear_error (&err);
    return GST_RTSP_ESYS;
  }
getnameinfo_failed:
  {
    GST_DEBUG ("getnameinfo failed: %s", err->message);
    g_clear_error (&err);
    if (!g_socket_close (client_sock, &err)) {
      GST_DEBUG ("Closing socket failed: %s", err->message);
      g_clear_error (&err);
    }
    g_object_unref (client_sock);
    return GST_RTSP_ERROR;
  }
}
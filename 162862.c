socket_client_event (GSocketClient * client, GSocketClientEvent event,
    GSocketConnectable * connectable, GTlsConnection * connection,
    GstRTSPConnection * rtspconn)
{
  if (event == G_SOCKET_CLIENT_TLS_HANDSHAKING) {
    GST_DEBUG ("TLS handshaking about to start...");

    g_signal_connect (connection, "accept-certificate",
        (GCallback) tls_accept_certificate, rtspconn);

    g_tls_connection_set_interaction (connection, rtspconn->tls_interaction);
  }
}
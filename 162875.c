tls_accept_certificate (GTlsConnection * conn, GTlsCertificate * peer_cert,
    GTlsCertificateFlags errors, GstRTSPConnection * rtspconn)
{
  GError *error = NULL;
  gboolean accept = FALSE;

  if (rtspconn->tls_database) {
    GSocketConnectable *peer_identity;
    GTlsCertificateFlags validation_flags;

    GST_DEBUG ("TLS peer certificate not accepted, checking user database...");

    peer_identity =
        g_tls_client_connection_get_server_identity (G_TLS_CLIENT_CONNECTION
        (conn));

    errors =
        g_tls_database_verify_chain (rtspconn->tls_database, peer_cert,
        G_TLS_DATABASE_PURPOSE_AUTHENTICATE_SERVER, peer_identity,
        g_tls_connection_get_interaction (conn), G_TLS_DATABASE_VERIFY_NONE,
        NULL, &error);

    if (error)
      goto verify_error;

    validation_flags = gst_rtsp_connection_get_tls_validation_flags (rtspconn);

    accept = ((errors & validation_flags) == 0);
    if (accept)
      GST_DEBUG ("Peer certificate accepted");
    else
      GST_DEBUG ("Peer certificate not accepted (errors: 0x%08X)", errors);
  }

  if (!accept && rtspconn->accept_certificate_func) {
    accept =
        rtspconn->accept_certificate_func (conn, peer_cert, errors,
        rtspconn->accept_certificate_user_data);
    GST_DEBUG ("Peer certificate %saccepted by accept-certificate function",
        accept ? "" : "not ");
  }

  return accept;

/* ERRORS */
verify_error:
  {
    GST_ERROR ("An error occurred while verifying the peer certificate: %s",
        error->message);
    g_clear_error (&error);
    return FALSE;
  }
}
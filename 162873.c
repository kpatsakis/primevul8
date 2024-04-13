gst_rtsp_connection_create (const GstRTSPUrl * url, GstRTSPConnection ** conn)
{
  GstRTSPConnection *newconn;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (url != NULL, GST_RTSP_EINVAL);

  newconn = g_new0 (GstRTSPConnection, 1);

  newconn->may_cancel = TRUE;
  newconn->cancellable = g_cancellable_new ();
  newconn->client = g_socket_client_new ();

  if (url->transports & GST_RTSP_LOWER_TRANS_TLS)
    g_socket_client_set_tls (newconn->client, TRUE);

  g_signal_connect (newconn->client, "event", (GCallback) socket_client_event,
      newconn);

  newconn->url = gst_rtsp_url_copy (url);
  newconn->timer = g_timer_new ();
  newconn->timeout = 60;
  newconn->cseq = 1;

  newconn->remember_session_id = TRUE;

  newconn->auth_method = GST_RTSP_AUTH_NONE;
  newconn->username = NULL;
  newconn->passwd = NULL;
  newconn->auth_params = NULL;
  newconn->version = 0;

  *conn = newconn;

  return GST_RTSP_OK;
}
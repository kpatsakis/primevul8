gst_rtsp_connection_free (GstRTSPConnection * conn)
{
  GstRTSPResult res;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  res = gst_rtsp_connection_close (conn);

  if (conn->cancellable)
    g_object_unref (conn->cancellable);
  if (conn->client)
    g_object_unref (conn->client);
  if (conn->tls_database)
    g_object_unref (conn->tls_database);
  if (conn->tls_interaction)
    g_object_unref (conn->tls_interaction);
  if (conn->accept_certificate_destroy_notify)
    conn->
        accept_certificate_destroy_notify (conn->accept_certificate_user_data);

  g_timer_destroy (conn->timer);
  gst_rtsp_url_free (conn->url);
  g_free (conn->proxy_host);
  g_free (conn);

  return res;
}
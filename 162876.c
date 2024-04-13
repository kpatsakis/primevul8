gst_rtsp_connection_connect_with_response (GstRTSPConnection * conn,
    GTimeVal * timeout, GstRTSPMessage * response)
{
  GstRTSPResult res;
  GSocketConnection *connection;
  GSocket *socket;
  GError *error = NULL;
  gchar *connection_uri, *request_uri, *remote_ip;
  GstClockTime to;
  guint16 url_port;
  GstRTSPUrl *url;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->url != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->stream0 == NULL, GST_RTSP_EINVAL);

  to = timeout ? GST_TIMEVAL_TO_TIME (*timeout) : 0;
  g_socket_client_set_timeout (conn->client,
      (to + GST_SECOND - 1) / GST_SECOND);

  url = conn->url;

  gst_rtsp_url_get_port (url, &url_port);

  if (conn->tunneled) {
    connection_uri = g_strdup_printf ("http://%s:%d%s%s%s", url->host, url_port,
        url->abspath, url->query ? "?" : "", url->query ? url->query : "");
  } else {
    connection_uri = gst_rtsp_url_get_request_uri (url);
  }

  if (conn->proxy_host) {
    connection = g_socket_client_connect_to_host (conn->client,
        conn->proxy_host, conn->proxy_port, conn->cancellable, &error);
    request_uri = g_strdup (connection_uri);
  } else {
    connection = g_socket_client_connect_to_uri (conn->client,
        connection_uri, url_port, conn->cancellable, &error);

    /* use the relative component of the uri for non-proxy connections */
    request_uri = g_strdup_printf ("%s%s%s", url->abspath,
        url->query ? "?" : "", url->query ? url->query : "");
  }
  if (connection == NULL)
    goto connect_failed;

  /* get remote address */
  socket = g_socket_connection_get_socket (connection);

  if (!collect_addresses (socket, &remote_ip, NULL, TRUE, &error))
    goto remote_address_failed;

  g_free (conn->remote_ip);
  conn->remote_ip = remote_ip;
  conn->stream0 = G_IO_STREAM (connection);
  conn->socket0 = socket;
  /* this is our read socket */
  conn->read_socket = conn->socket0;
  conn->write_socket = conn->socket0;
  conn->input_stream = g_io_stream_get_input_stream (conn->stream0);
  conn->output_stream = g_io_stream_get_output_stream (conn->stream0);
  conn->control_stream = NULL;

  if (conn->tunneled) {
    res = setup_tunneling (conn, timeout, request_uri, response);
    if (res != GST_RTSP_OK)
      goto tunneling_failed;
  }
  g_free (connection_uri);
  g_free (request_uri);

  return GST_RTSP_OK;

  /* ERRORS */
connect_failed:
  {
    GST_ERROR ("failed to connect: %s", error->message);
    g_clear_error (&error);
    g_free (connection_uri);
    g_free (request_uri);
    return GST_RTSP_ERROR;
  }
remote_address_failed:
  {
    GST_ERROR ("failed to connect: %s", error->message);
    g_object_unref (connection);
    g_clear_error (&error);
    g_free (connection_uri);
    g_free (request_uri);
    return GST_RTSP_ERROR;
  }
tunneling_failed:
  {
    GST_ERROR ("failed to setup tunneling");
    g_free (connection_uri);
    g_free (request_uri);
    return res;
  }
}
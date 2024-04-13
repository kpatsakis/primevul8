gst_rtsp_connection_create_from_socket (GSocket * socket, const gchar * ip,
    guint16 port, const gchar * initial_buffer, GstRTSPConnection ** conn)
{
  GstRTSPConnection *newconn = NULL;
  GstRTSPUrl *url;
  GstRTSPResult res;
  GError *err = NULL;
  gchar *local_ip;
  GIOStream *stream;

  g_return_val_if_fail (G_IS_SOCKET (socket), GST_RTSP_EINVAL);
  g_return_val_if_fail (ip != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  if (!collect_addresses (socket, &local_ip, NULL, FALSE, &err))
    goto getnameinfo_failed;

  /* create a url for the client address */
  url = g_new0 (GstRTSPUrl, 1);
  url->host = g_strdup (ip);
  url->port = port;

  /* now create the connection object */
  GST_RTSP_CHECK (gst_rtsp_connection_create (url, &newconn), newconn_failed);
  gst_rtsp_url_free (url);

  stream = G_IO_STREAM (g_socket_connection_factory_create_connection (socket));

  /* both read and write initially */
  newconn->server = TRUE;
  newconn->socket0 = socket;
  newconn->stream0 = stream;
  newconn->write_socket = newconn->read_socket = newconn->socket0;
  newconn->input_stream = g_io_stream_get_input_stream (stream);
  newconn->output_stream = g_io_stream_get_output_stream (stream);
  newconn->control_stream = NULL;
  newconn->remote_ip = g_strdup (ip);
  newconn->local_ip = local_ip;
  newconn->initial_buffer = g_strdup (initial_buffer);

  *conn = newconn;

  return GST_RTSP_OK;

  /* ERRORS */
getnameinfo_failed:
  {
    GST_ERROR ("failed to get local address: %s", err->message);
    g_clear_error (&err);
    return GST_RTSP_ERROR;
  }
newconn_failed:
  {
    GST_ERROR ("failed to make connection");
    g_free (local_ip);
    gst_rtsp_url_free (url);
    return res;
  }
}
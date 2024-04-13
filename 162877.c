setup_tunneling (GstRTSPConnection * conn, GTimeVal * timeout, gchar * uri,
    GstRTSPMessage * response)
{
  gint i;
  GstRTSPResult res;
  gchar *value;
  guint16 url_port;
  GstRTSPMessage *msg;
  gboolean old_http;
  GstRTSPUrl *url;
  GError *error = NULL;
  GSocketConnection *connection;
  GSocket *socket;
  gchar *connection_uri = NULL;
  gchar *request_uri = NULL;
  gchar *host = NULL;

  url = conn->url;

  gst_rtsp_url_get_port (url, &url_port);
  host = g_strdup_printf ("%s:%d", url->host, url_port);

  /* create a random sessionid */
  for (i = 0; i < TUNNELID_LEN; i++)
    conn->tunnelid[i] = g_random_int_range ('a', 'z');
  conn->tunnelid[TUNNELID_LEN - 1] = '\0';

  /* create the GET request for the read connection */
  GST_RTSP_CHECK (gst_rtsp_message_new_request (&msg, GST_RTSP_GET, uri),
      no_message);
  msg->type = GST_RTSP_MESSAGE_HTTP_REQUEST;

  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_X_SESSIONCOOKIE,
      conn->tunnelid);
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_ACCEPT,
      "application/x-rtsp-tunnelled");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CACHE_CONTROL, "no-cache");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_PRAGMA, "no-cache");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_HOST, host);

  /* we need to temporarily set conn->tunneled to FALSE to prevent the HTTP
   * request from being base64 encoded */
  conn->tunneled = FALSE;
  GST_RTSP_CHECK (gst_rtsp_connection_send (conn, msg, timeout), write_failed);
  gst_rtsp_message_free (msg);
  conn->tunneled = TRUE;

  /* receive the response to the GET request */
  /* we need to temporarily set manual_http to TRUE since
   * gst_rtsp_connection_receive() will treat the HTTP response as a parsing
   * failure otherwise */
  old_http = conn->manual_http;
  conn->manual_http = TRUE;
  GST_RTSP_CHECK (gst_rtsp_connection_receive (conn, response, timeout),
      read_failed);
  conn->manual_http = old_http;

  if (response->type != GST_RTSP_MESSAGE_HTTP_RESPONSE ||
      response->type_data.response.code != GST_RTSP_STS_OK)
    goto wrong_result;

  if (gst_rtsp_message_get_header (response, GST_RTSP_HDR_X_SERVER_IP_ADDRESS,
          &value, 0) == GST_RTSP_OK) {
    g_free (url->host);
    url->host = g_strdup (value);
    g_free (conn->remote_ip);
    conn->remote_ip = g_strdup (value);
  }

  connection_uri = g_strdup_printf ("http://%s:%d%s%s%s", url->host, url_port,
      url->abspath, url->query ? "?" : "", url->query ? url->query : "");

  /* connect to the host/port */
  if (conn->proxy_host) {
    connection = g_socket_client_connect_to_host (conn->client,
        conn->proxy_host, conn->proxy_port, conn->cancellable, &error);
    request_uri = g_strdup (connection_uri);
  } else {
    connection = g_socket_client_connect_to_uri (conn->client,
        connection_uri, 0, conn->cancellable, &error);
    request_uri =
        g_strdup_printf ("%s%s%s", url->abspath,
        url->query ? "?" : "", url->query ? url->query : "");
  }
  if (connection == NULL)
    goto connect_failed;

  socket = g_socket_connection_get_socket (connection);

  /* get remote address */
  g_free (conn->remote_ip);
  conn->remote_ip = NULL;

  if (!collect_addresses (socket, &conn->remote_ip, NULL, TRUE, &error))
    goto remote_address_failed;

  /* this is now our writing socket */
  conn->stream1 = G_IO_STREAM (connection);
  conn->socket1 = socket;
  conn->write_socket = conn->socket1;
  conn->output_stream = g_io_stream_get_output_stream (conn->stream1);
  conn->control_stream = NULL;

  /* create the POST request for the write connection */
  GST_RTSP_CHECK (gst_rtsp_message_new_request (&msg, GST_RTSP_POST,
          request_uri), no_message);
  msg->type = GST_RTSP_MESSAGE_HTTP_REQUEST;

  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_X_SESSIONCOOKIE,
      conn->tunnelid);
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_ACCEPT,
      "application/x-rtsp-tunnelled");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CONTENT_TYPE,
      "application/x-rtsp-tunnelled");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CACHE_CONTROL, "no-cache");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_PRAGMA, "no-cache");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_EXPIRES,
      "Sun, 9 Jan 1972 00:00:00 GMT");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CONTENT_LENGTH, "32767");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_HOST, host);

  /* we need to temporarily set conn->tunneled to FALSE to prevent the HTTP
   * request from being base64 encoded */
  conn->tunneled = FALSE;
  GST_RTSP_CHECK (gst_rtsp_connection_send (conn, msg, timeout), write_failed);
  gst_rtsp_message_free (msg);
  conn->tunneled = TRUE;

exit:
  g_free (connection_uri);
  g_free (request_uri);
  g_free (host);

  return res;

  /* ERRORS */
no_message:
  {
    GST_ERROR ("failed to create request (%d)", res);
    goto exit;
  }
write_failed:
  {
    GST_ERROR ("write failed (%d)", res);
    gst_rtsp_message_free (msg);
    conn->tunneled = TRUE;
    goto exit;
  }
read_failed:
  {
    GST_ERROR ("read failed (%d)", res);
    conn->manual_http = FALSE;
    goto exit;
  }
wrong_result:
  {
    GST_ERROR ("got failure response %d %s",
        response->type_data.response.code, response->type_data.response.reason);
    res = GST_RTSP_ERROR;
    goto exit;
  }
connect_failed:
  {
    GST_ERROR ("failed to connect: %s", error->message);
    res = GST_RTSP_ERROR;
    g_clear_error (&error);
    goto exit;
  }
remote_address_failed:
  {
    GST_ERROR ("failed to resolve address: %s", error->message);
    g_object_unref (connection);
    g_clear_error (&error);
    return GST_RTSP_ERROR;
  }
}
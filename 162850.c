gst_rtsp_connection_get_tls (GstRTSPConnection * conn, GError ** error)
{
  GTlsConnection *result;

  if (G_IS_TLS_CONNECTION (conn->stream0)) {
    /* we already had one, return it */
    result = G_TLS_CONNECTION (conn->stream0);
  } else if (conn->server) {
    /* no TLS connection but we are server, make one */
    result = (GTlsConnection *)
        g_tls_server_connection_new (conn->stream0, NULL, error);
    if (result) {
      g_object_unref (conn->stream0);
      conn->stream0 = G_IO_STREAM (result);
      conn->input_stream = g_io_stream_get_input_stream (conn->stream0);
      conn->output_stream = g_io_stream_get_output_stream (conn->stream0);
    }
  } else {
    /* client */
    result = NULL;
    g_set_error (error, GST_LIBRARY_ERROR, GST_LIBRARY_ERROR_FAILED,
        "client not connected with TLS");
  }
  return result;
}
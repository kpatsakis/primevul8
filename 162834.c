gst_rtsp_connection_close (GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  /* last unref closes the connection we don't want to explicitly close here
   * because these sockets might have been provided at construction */
  if (conn->stream0) {
    g_object_unref (conn->stream0);
    conn->stream0 = NULL;
    conn->socket0 = NULL;
  }
  if (conn->stream1) {
    g_object_unref (conn->stream1);
    conn->stream1 = NULL;
    conn->socket1 = NULL;
  }

  /* these were owned by the stream */
  conn->input_stream = NULL;
  conn->output_stream = NULL;
  conn->control_stream = NULL;

  g_free (conn->remote_ip);
  conn->remote_ip = NULL;
  g_free (conn->local_ip);
  conn->local_ip = NULL;

  conn->read_ahead = 0;

  g_free (conn->initial_buffer);
  conn->initial_buffer = NULL;
  conn->initial_buffer_offset = 0;

  conn->write_socket = NULL;
  conn->read_socket = NULL;
  conn->tunneled = FALSE;
  conn->tstate = TUNNEL_STATE_NONE;
  conn->ctxp = NULL;
  g_free (conn->username);
  conn->username = NULL;
  g_free (conn->passwd);
  conn->passwd = NULL;
  gst_rtsp_connection_clear_auth_params (conn);
  conn->timeout = 60;
  conn->cseq = 0;
  conn->session_id[0] = '\0';

  return GST_RTSP_OK;
}
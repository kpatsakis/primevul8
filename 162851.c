gst_rtsp_connection_do_tunnel (GstRTSPConnection * conn,
    GstRTSPConnection * conn2)
{
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  if (conn2 != NULL) {
    GstRTSPTunnelState ts1 = conn->tstate;
    GstRTSPTunnelState ts2 = conn2->tstate;

    g_return_val_if_fail ((ts1 == TUNNEL_STATE_GET && ts2 == TUNNEL_STATE_POST)
        || (ts1 == TUNNEL_STATE_POST && ts2 == TUNNEL_STATE_GET),
        GST_RTSP_EINVAL);
    g_return_val_if_fail (!memcmp (conn2->tunnelid, conn->tunnelid,
            TUNNELID_LEN), GST_RTSP_EINVAL);

    /* both connections have socket0 as the read/write socket */
    if (ts1 == TUNNEL_STATE_GET) {
      /* conn2 is the HTTP POST channel. take its socket and set it as read
       * socket in conn */
      conn->socket1 = conn2->socket0;
      conn->stream1 = conn2->stream0;
      conn->input_stream = conn2->input_stream;
      conn->control_stream = g_io_stream_get_input_stream (conn->stream0);
      conn2->output_stream = NULL;
    } else {
      /* conn2 is the HTTP GET channel. take its socket and set it as write
       * socket in conn */
      conn->socket1 = conn->socket0;
      conn->stream1 = conn->stream0;
      conn->socket0 = conn2->socket0;
      conn->stream0 = conn2->stream0;
      conn->output_stream = conn2->output_stream;
      conn->control_stream = g_io_stream_get_input_stream (conn->stream0);
    }

    /* clean up some of the state of conn2 */
    g_cancellable_cancel (conn2->cancellable);
    conn2->write_socket = conn2->read_socket = NULL;
    conn2->socket0 = NULL;
    conn2->stream0 = NULL;
    conn2->socket1 = NULL;
    conn2->stream1 = NULL;
    conn2->input_stream = NULL;
    conn2->control_stream = NULL;
    g_object_unref (conn2->cancellable);
    conn2->cancellable = NULL;

    /* We make socket0 the write socket and socket1 the read socket. */
    conn->write_socket = conn->socket0;
    conn->read_socket = conn->socket1;

    conn->tstate = TUNNEL_STATE_COMPLETE;

    g_free (conn->initial_buffer);
    conn->initial_buffer = conn2->initial_buffer;
    conn2->initial_buffer = NULL;
    conn->initial_buffer_offset = conn2->initial_buffer_offset;
  }

  /* we need base64 decoding for the readfd */
  conn->ctx.state = 0;
  conn->ctx.save = 0;
  conn->ctx.cout = 0;
  conn->ctx.coutl = 0;
  conn->ctxp = &conn->ctx;

  return GST_RTSP_OK;
}
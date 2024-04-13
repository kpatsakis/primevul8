gst_rtsp_source_dispatch_read (GPollableInputStream * stream,
    GstRTSPWatch * watch)
{
  GstRTSPResult res = GST_RTSP_ERROR;
  GstRTSPConnection *conn = watch->conn;

  /* if this connection was already closed, stop now */
  if (G_POLLABLE_INPUT_STREAM (conn->input_stream) != stream)
    goto eof;

  res = build_next (&watch->builder, &watch->message, conn, FALSE);
  if (res == GST_RTSP_EINTR)
    goto done;
  else if (G_UNLIKELY (res == GST_RTSP_EEOF)) {
    g_mutex_lock (&watch->mutex);
    if (watch->readsrc) {
      if (!g_source_is_destroyed ((GSource *) watch))
        g_source_remove_child_source ((GSource *) watch, watch->readsrc);
      g_source_unref (watch->readsrc);
      watch->readsrc = NULL;
    }

    if (conn->stream1) {
      g_object_unref (conn->stream1);
      conn->stream1 = NULL;
      conn->socket1 = NULL;
      conn->input_stream = NULL;
    }
    g_mutex_unlock (&watch->mutex);

    /* When we are in tunnelled mode, the read socket can be closed and we
     * should be prepared for a new POST method to reopen it */
    if (conn->tstate == TUNNEL_STATE_COMPLETE) {
      /* remove the read connection for the tunnel */
      /* we accept a new POST request */
      conn->tstate = TUNNEL_STATE_GET;
      /* and signal that we lost our tunnel */
      if (watch->funcs.tunnel_lost)
        res = watch->funcs.tunnel_lost (watch, watch->user_data);
      /* we add read source on the write socket able to detect when client closes get channel in tunneled mode */
      g_mutex_lock (&watch->mutex);
      if (watch->conn->control_stream && !watch->controlsrc) {
        watch->controlsrc =
            g_pollable_input_stream_create_source (G_POLLABLE_INPUT_STREAM
            (watch->conn->control_stream), NULL);
        g_source_set_callback (watch->controlsrc,
            (GSourceFunc) gst_rtsp_source_dispatch_read_get_channel, watch,
            NULL);
        g_source_add_child_source ((GSource *) watch, watch->controlsrc);
      }
      g_mutex_unlock (&watch->mutex);
      goto read_done;
    } else
      goto eof;
  } else if (G_LIKELY (res == GST_RTSP_OK)) {
    if (!conn->manual_http &&
        watch->message.type == GST_RTSP_MESSAGE_HTTP_REQUEST) {
      if (conn->tstate == TUNNEL_STATE_NONE &&
          watch->message.type_data.request.method == GST_RTSP_GET) {
        GstRTSPMessage *response;
        GstRTSPStatusCode code;

        conn->tstate = TUNNEL_STATE_GET;

        if (watch->funcs.tunnel_start)
          code = watch->funcs.tunnel_start (watch, watch->user_data);
        else
          code = GST_RTSP_STS_OK;

        /* queue the response */
        response = gen_tunnel_reply (conn, code, &watch->message);
        if (watch->funcs.tunnel_http_response)
          watch->funcs.tunnel_http_response (watch, &watch->message, response,
              watch->user_data);
        gst_rtsp_watch_send_message (watch, response, NULL);
        gst_rtsp_message_free (response);
        goto read_done;
      } else if (conn->tstate == TUNNEL_STATE_NONE &&
          watch->message.type_data.request.method == GST_RTSP_POST) {
        conn->tstate = TUNNEL_STATE_POST;

        /* in the callback the connection should be tunneled with the
         * GET connection */
        if (watch->funcs.tunnel_complete) {
          watch->funcs.tunnel_complete (watch, watch->user_data);
        }
        goto read_done;
      }
    }
  } else
    goto read_error;

  if (!conn->manual_http) {
    /* if manual HTTP support is not enabled, then restore the message to
     * what it would have looked like without the support for parsing HTTP
     * messages being present */
    if (watch->message.type == GST_RTSP_MESSAGE_HTTP_REQUEST) {
      watch->message.type = GST_RTSP_MESSAGE_REQUEST;
      watch->message.type_data.request.method = GST_RTSP_INVALID;
      if (watch->message.type_data.request.version != GST_RTSP_VERSION_1_0)
        watch->message.type_data.request.version = GST_RTSP_VERSION_INVALID;
      res = GST_RTSP_EPARSE;
    } else if (watch->message.type == GST_RTSP_MESSAGE_HTTP_RESPONSE) {
      watch->message.type = GST_RTSP_MESSAGE_RESPONSE;
      if (watch->message.type_data.response.version != GST_RTSP_VERSION_1_0)
        watch->message.type_data.response.version = GST_RTSP_VERSION_INVALID;
      res = GST_RTSP_EPARSE;
    }
  }
  if (G_LIKELY (res != GST_RTSP_OK))
    goto read_error;

  if (watch->funcs.message_received)
    watch->funcs.message_received (watch, &watch->message, watch->user_data);

read_done:
  gst_rtsp_message_unset (&watch->message);
  build_reset (&watch->builder);

done:
  return TRUE;

  /* ERRORS */
eof:
  {
    if (watch->funcs.closed)
      watch->funcs.closed (watch, watch->user_data);

    /* we closed the read connection, stop the watch now */
    watch->keep_running = FALSE;

    /* always stop when the input returns EOF in non-tunneled mode */
    return FALSE;
  }
read_error:
  {
    if (watch->funcs.error_full)
      watch->funcs.error_full (watch, res, &watch->message,
          0, watch->user_data);
    else if (watch->funcs.error)
      watch->funcs.error (watch, res, watch->user_data);

    goto eof;
  }
}
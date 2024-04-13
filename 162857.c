gst_rtsp_connection_receive (GstRTSPConnection * conn, GstRTSPMessage * message,
    GTimeVal * timeout)
{
  GstRTSPResult res;
  GstRTSPBuilder builder;
  GstClockTime to;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (message != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->read_socket != NULL, GST_RTSP_EINVAL);

  /* configure timeout if any */
  to = timeout ? GST_TIMEVAL_TO_TIME (*timeout) : 0;

  g_socket_set_timeout (conn->read_socket, (to + GST_SECOND - 1) / GST_SECOND);
  memset (&builder, 0, sizeof (GstRTSPBuilder));
  res = build_next (&builder, message, conn, TRUE);
  g_socket_set_timeout (conn->read_socket, 0);

  if (G_UNLIKELY (res != GST_RTSP_OK))
    goto read_error;

  if (!conn->manual_http) {
    if (message->type == GST_RTSP_MESSAGE_HTTP_REQUEST) {
      if (conn->tstate == TUNNEL_STATE_NONE &&
          message->type_data.request.method == GST_RTSP_GET) {
        GstRTSPMessage *response;

        conn->tstate = TUNNEL_STATE_GET;

        /* tunnel GET request, we can reply now */
        response = gen_tunnel_reply (conn, GST_RTSP_STS_OK, message);
        res = gst_rtsp_connection_send (conn, response, timeout);
        gst_rtsp_message_free (response);
        if (res == GST_RTSP_OK)
          res = GST_RTSP_ETGET;
        goto cleanup;
      } else if (conn->tstate == TUNNEL_STATE_NONE &&
          message->type_data.request.method == GST_RTSP_POST) {
        conn->tstate = TUNNEL_STATE_POST;

        /* tunnel POST request, the caller now has to link the two
         * connections. */
        res = GST_RTSP_ETPOST;
        goto cleanup;
      } else {
        res = GST_RTSP_EPARSE;
        goto cleanup;
      }
    } else if (message->type == GST_RTSP_MESSAGE_HTTP_RESPONSE) {
      res = GST_RTSP_EPARSE;
      goto cleanup;
    }
  }

  /* we have a message here */
  build_reset (&builder);

  return GST_RTSP_OK;

  /* ERRORS */
read_error:
cleanup:
  {
    build_reset (&builder);
    gst_rtsp_message_unset (message);
    return res;
  }
}
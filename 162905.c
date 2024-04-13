gst_rtsp_connection_poll (GstRTSPConnection * conn, GstRTSPEvent events,
    GstRTSPEvent * revents, GTimeVal * timeout)
{
  GMainContext *ctx;
  GSource *rs, *ws, *ts;
  GIOCondition condition;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (events != 0, GST_RTSP_EINVAL);
  g_return_val_if_fail (revents != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->read_socket != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->write_socket != NULL, GST_RTSP_EINVAL);

  ctx = g_main_context_new ();

  /* configure timeout if any */
  if (timeout) {
    ts = g_timeout_source_new (GST_TIMEVAL_TO_TIME (*timeout) / GST_MSECOND);
    g_source_set_dummy_callback (ts);
    g_source_attach (ts, ctx);
    g_source_unref (ts);
  }

  if (events & GST_RTSP_EV_READ) {
    rs = g_socket_create_source (conn->read_socket, G_IO_IN | G_IO_PRI,
        conn->cancellable);
    g_source_set_dummy_callback (rs);
    g_source_attach (rs, ctx);
    g_source_unref (rs);
  }

  if (events & GST_RTSP_EV_WRITE) {
    ws = g_socket_create_source (conn->write_socket, G_IO_OUT,
        conn->cancellable);
    g_source_set_dummy_callback (ws);
    g_source_attach (ws, ctx);
    g_source_unref (ws);
  }

  /* Returns after handling all pending events */
  while (!g_main_context_iteration (ctx, TRUE));

  g_main_context_unref (ctx);

  *revents = 0;
  if (events & GST_RTSP_EV_READ) {
    condition = g_socket_condition_check (conn->read_socket,
        G_IO_IN | G_IO_PRI);
    if ((condition & G_IO_IN) || (condition & G_IO_PRI))
      *revents |= GST_RTSP_EV_READ;
  }
  if (events & GST_RTSP_EV_WRITE) {
    condition = g_socket_condition_check (conn->write_socket, G_IO_OUT);
    if ((condition & G_IO_OUT))
      *revents |= GST_RTSP_EV_WRITE;
  }

  if (*revents == 0)
    return GST_RTSP_ETIMEOUT;

  return GST_RTSP_OK;
}
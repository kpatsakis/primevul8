gst_rtsp_source_dispatch_read_get_channel (GPollableInputStream * stream,
    GstRTSPWatch * watch)
{
  gssize count;
  guint8 buffer[1024];
  GError *error = NULL;

  /* try to read in order to be able to detect errors, we read 1k in case some
   * client actually decides to send data on the GET channel */
  count = g_pollable_input_stream_read_nonblocking (stream, buffer, 1024, NULL,
      &error);
  if (count == 0) {
    /* other end closed the socket */
    goto eof;
  }

  if (count < 0) {
    GST_DEBUG ("%s", error->message);
    if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK) ||
        g_error_matches (error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT)) {
      g_clear_error (&error);
      goto done;
    }
    g_clear_error (&error);
    goto read_error;
  }

  /* client sent data on the GET channel, ignore it */

done:
  return TRUE;

  /* ERRORS */
eof:
  {
    if (watch->funcs.closed)
      watch->funcs.closed (watch, watch->user_data);

    /* the read connection was closed, stop the watch now */
    watch->keep_running = FALSE;

    return FALSE;
  }
read_error:
  {
    if (watch->funcs.error_full)
      watch->funcs.error_full (watch, GST_RTSP_ESYS, &watch->message,
          0, watch->user_data);
    else if (watch->funcs.error)
      watch->funcs.error (watch, GST_RTSP_ESYS, watch->user_data);

    goto eof;
  }
}
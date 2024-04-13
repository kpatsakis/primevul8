write_bytes (GOutputStream * stream, const guint8 * buffer, guint * idx,
    guint size, gboolean block, GCancellable * cancellable)
{
  guint left;
  gssize r;
  GError *err = NULL;

  if (G_UNLIKELY (*idx > size))
    return GST_RTSP_ERROR;

  left = size - *idx;

  while (left) {
    if (block)
      r = g_output_stream_write (stream, (gchar *) & buffer[*idx], left,
          cancellable, &err);
    else
      r = g_pollable_output_stream_write_nonblocking (G_POLLABLE_OUTPUT_STREAM
          (stream), (gchar *) & buffer[*idx], left, cancellable, &err);
    if (G_UNLIKELY (r < 0))
      goto error;

    left -= r;
    *idx += r;
  }
  return GST_RTSP_OK;

  /* ERRORS */
error:
  {
    if (G_UNLIKELY (r == 0))
      return GST_RTSP_EEOF;

    if (!g_error_matches (err, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK))
      GST_WARNING ("%s", err->message);
    else
      GST_DEBUG ("%s", err->message);

    if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
      g_clear_error (&err);
      return GST_RTSP_EINTR;
    } else if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)) {
      g_clear_error (&err);
      return GST_RTSP_EINTR;
    } else if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_TIMED_OUT)) {
      g_clear_error (&err);
      return GST_RTSP_ETIMEOUT;
    }
    g_clear_error (&err);
    return GST_RTSP_ESYS;
  }
}
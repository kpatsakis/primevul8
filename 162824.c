read_bytes (GstRTSPConnection * conn, guint8 * buffer, guint * idx, guint size,
    gboolean block)
{
  guint left;
  gint r;
  GError *err = NULL;

  if (G_UNLIKELY (*idx > size))
    return GST_RTSP_ERROR;

  left = size - *idx;

  while (left) {
    r = fill_bytes (conn, &buffer[*idx], left, block, &err);
    if (G_UNLIKELY (r <= 0))
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
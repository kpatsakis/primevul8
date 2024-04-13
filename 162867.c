writev_bytes (GOutputStream * stream, GOutputVector * vectors, gint n_vectors,
    gsize * bytes_written, gboolean block, GCancellable * cancellable)
{
  gsize _bytes_written = 0;
  gsize written;
  GError *err = NULL;
  GPollableReturn res = G_POLLABLE_RETURN_OK;

  while (n_vectors > 0) {
    if (block) {
      if (G_UNLIKELY (!g_output_stream_writev (stream, vectors, n_vectors,
                  &written, cancellable, &err))) {
        /* This will never return G_IO_ERROR_WOULD_BLOCK */
        res = G_POLLABLE_RETURN_FAILED;
        goto error;
      }
    } else {
      res =
          g_pollable_output_stream_writev_nonblocking (G_POLLABLE_OUTPUT_STREAM
          (stream), vectors, n_vectors, &written, cancellable, &err);

      if (res != G_POLLABLE_RETURN_OK) {
        g_assert (written == 0);
        goto error;
      }
    }
    _bytes_written += written;

    /* skip vectors that have been written in full */
    while (written > 0 && written >= vectors[0].size) {
      written -= vectors[0].size;
      ++vectors;
      --n_vectors;
    }

    /* skip partially written vector data */
    if (written > 0) {
      vectors[0].size -= written;
      vectors[0].buffer = ((guint8 *) vectors[0].buffer) + written;
    }
  }

  *bytes_written = _bytes_written;

  return GST_RTSP_OK;

  /* ERRORS */
error:
  {
    *bytes_written = _bytes_written;

    if (err)
      GST_WARNING ("%s", err->message);
    if (res == G_POLLABLE_RETURN_WOULD_BLOCK) {
      g_assert (!err);
      return GST_RTSP_EINTR;
    } else if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
      g_clear_error (&err);
      return GST_RTSP_EINTR;
    } else if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_TIMED_OUT)) {
      g_clear_error (&err);
      return GST_RTSP_ETIMEOUT;
    } else if (G_UNLIKELY (written == 0)) {
      g_clear_error (&err);
      return GST_RTSP_EEOF;
    }

    g_clear_error (&err);
    return GST_RTSP_ESYS;
  }
}
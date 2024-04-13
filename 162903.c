writev_bytes (GOutputStream * stream, GOutputVector * vectors, gint n_vectors,
    gsize * bytes_written, gboolean block, GCancellable * cancellable)
{
  gsize _bytes_written = 0;
  guint written;
  gint i;
  GstRTSPResult res = GST_RTSP_OK;

  for (i = 0; i < n_vectors; i++) {
    written = 0;
    res =
        write_bytes (stream, vectors[i].buffer, &written, vectors[i].size,
        block, cancellable);
    _bytes_written += written;
    if (G_UNLIKELY (res != GST_RTSP_OK))
      break;
  }

  *bytes_written = _bytes_written;

  return res;
}
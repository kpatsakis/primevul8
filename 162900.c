fill_raw_bytes (GstRTSPConnection * conn, guint8 * buffer, guint size,
    gboolean block, GError ** err)
{
  gint out = 0;

  if (G_UNLIKELY (conn->initial_buffer != NULL)) {
    gsize left = strlen (&conn->initial_buffer[conn->initial_buffer_offset]);

    out = MIN (left, size);
    memcpy (buffer, &conn->initial_buffer[conn->initial_buffer_offset], out);

    if (left == (gsize) out) {
      g_free (conn->initial_buffer);
      conn->initial_buffer = NULL;
      conn->initial_buffer_offset = 0;
    } else
      conn->initial_buffer_offset += out;
  }

  if (G_LIKELY (size > (guint) out)) {
    gssize r;
    gsize count = size - out;
    if (block)
      r = g_input_stream_read (conn->input_stream, (gchar *) & buffer[out],
          count, conn->may_cancel ? conn->cancellable : NULL, err);
    else
      r = g_pollable_input_stream_read_nonblocking (G_POLLABLE_INPUT_STREAM
          (conn->input_stream), (gchar *) & buffer[out], count,
          conn->may_cancel ? conn->cancellable : NULL, err);

    if (G_UNLIKELY (r < 0)) {
      if (out == 0) {
        /* propagate the error */
        out = r;
      } else {
        /* we have some data ignore error */
        g_clear_error (err);
      }
    } else
      out += r;
  }

  return out;
}
fill_bytes (GstRTSPConnection * conn, guint8 * buffer, guint size,
    gboolean block, GError ** err)
{
  DecodeCtx *ctx = conn->ctxp;
  gint out = 0;

  if (ctx) {
    while (size > 0) {
      guint8 in[sizeof (ctx->out) * 4 / 3];
      gint r;

      while (size > 0 && ctx->cout < ctx->coutl) {
        /* we have some leftover bytes */
        *buffer++ = ctx->out[ctx->cout++];
        size--;
        out++;
      }

      /* got what we needed? */
      if (size == 0)
        break;

      /* try to read more bytes */
      r = fill_raw_bytes (conn, in, sizeof (in), block, err);
      if (r <= 0) {
        if (out == 0) {
          out = r;
        } else {
          /* we have some data ignore error */
          g_clear_error (err);
        }
        break;
      }

      ctx->cout = 0;
      ctx->coutl =
          g_base64_decode_step ((gchar *) in, r, ctx->out, &ctx->state,
          &ctx->save);
    }
  } else {
    out = fill_raw_bytes (conn, buffer, size, block, err);
  }

  return out;
}
flush_buffer(smtp_outblock * outblock, int mode)
{
int rc;
int n = outblock->ptr - outblock->buffer;
BOOL more = mode == SCMD_MORE;

HDEBUG(D_transport|D_acl) debug_printf_indent("cmd buf flush %d bytes%s\n", n,
  more ? " (more expected)" : "");

#ifdef SUPPORT_TLS
if (outblock->cctx->tls_ctx)
  rc = tls_write(outblock->cctx->tls_ctx, outblock->buffer, n, more);
else
#endif

  {
  if (outblock->conn_args)
    {
    blob early_data = { .data = outblock->buffer, .len = n };

    /* We ignore the more-flag if we're doing a connect with early-data, which
    means we won't get BDAT+data. A pity, but wise due to the idempotency
    requirement: TFO with data can, in rare cases, replay the data to the
    receiver. */

    if (  (outblock->cctx->sock = smtp_connect(outblock->conn_args, &early_data))
       < 0)
      return FALSE;
    outblock->conn_args = NULL;
    rc = n;
    }
  else

    rc = send(outblock->cctx->sock, outblock->buffer, n,
#ifdef MSG_MORE
	      more ? MSG_MORE : 0
#else
	      0
#endif
	     );
  }

if (rc <= 0)
  {
  HDEBUG(D_transport|D_acl) debug_printf_indent("send failed: %s\n", strerror(errno));
  return FALSE;
  }

outblock->ptr = outblock->buffer;
outblock->cmd_count = 0;
return TRUE;
}
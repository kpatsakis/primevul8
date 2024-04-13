transport_write_block(transport_ctx * tctx, uschar *block, int len, BOOL more)
{
if (!(tctx->options & topt_output_string))
  return transport_write_block_fd(tctx, block, len, more);

/* Write to expanding-string.  NOTE: not NUL-terminated */

if (!tctx->u.msg)
  tctx->u.msg = string_get(1024);

tctx->u.msg = string_catn(tctx->u.msg, block, len);
return TRUE;
}
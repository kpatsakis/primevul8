void imap_keepalive (void)
{
  CONNECTION *conn;
  CONTEXT *ctx = NULL;
  IMAP_DATA *idata;

  conn = mutt_socket_head ();
  while (conn)
  {
    if (conn->account.type == MUTT_ACCT_TYPE_IMAP)
    {
      int need_free = 0;

      idata = (IMAP_DATA*) conn->data;

      if (idata->state >= IMAP_AUTHENTICATED
	  && time(NULL) >= idata->lastread + ImapKeepalive)
      {
	if (idata->ctx)
	  ctx = idata->ctx;
	else
	{
	  ctx = safe_calloc (1, sizeof (CONTEXT));
	  ctx->data = idata;
	  /* imap_close_mailbox will set ctx->iadata->ctx to NULL, so we can't
	   * rely on the value of iadata->ctx to determine if this placeholder
	   * context needs to be freed.
	   */
	  need_free = 1;
	}
	/* if the imap connection closes during this call, ctx may be invalid
	 * after this point, and thus should not be read.
	 */
	imap_check_mailbox (ctx, NULL, 1);
	if (need_free)
	  FREE (&ctx);
      }
    }

    conn = conn->next;
  }
}
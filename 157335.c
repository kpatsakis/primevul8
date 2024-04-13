IMAP_DATA* imap_new_idata (void)
{
  IMAP_DATA* idata = safe_calloc (1, sizeof (IMAP_DATA));

  if (!idata)
    return NULL;

  if (!(idata->cmdbuf = mutt_buffer_new ()))
    FREE (&idata);

  idata->cmdslots = ImapPipelineDepth + 2;
  if (!(idata->cmds = safe_calloc(idata->cmdslots, sizeof(*idata->cmds))))
  {
    mutt_buffer_free(&idata->cmdbuf);
    FREE (&idata);
  }

  return idata;
}
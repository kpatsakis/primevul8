zlog_reset_file (struct zlog *zl)
{
  if (zl == NULL)
    zl = zlog_default;

  if (zl->fp)
    fclose (zl->fp);
  zl->fp = NULL;
  logfile_fd = -1;
  zl->maxlvl[ZLOG_DEST_FILE] = ZLOG_DISABLED;

  if (zl->filename)
    free (zl->filename);
  zl->filename = NULL;

  return 1;
}
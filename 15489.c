zlog_rotate (struct zlog *zl)
{
  int level;

  if (zl == NULL)
    zl = zlog_default;

  if (zl->fp)
    fclose (zl->fp);
  zl->fp = NULL;
  logfile_fd = -1;
  level = zl->maxlvl[ZLOG_DEST_FILE];
  zl->maxlvl[ZLOG_DEST_FILE] = ZLOG_DISABLED;

  if (zl->filename)
    {
      mode_t oldumask;
      int save_errno;

      oldumask = umask (0777 & ~LOGFILE_MASK);
      zl->fp = fopen (zl->filename, "a");
      save_errno = errno;
      umask(oldumask);
      if (zl->fp == NULL)
        {
	  zlog_err("Log rotate failed: cannot open file %s for append: %s",
	  	   zl->filename, safe_strerror(save_errno));
	  return -1;
        }	
      logfile_fd = fileno(zl->fp);
      zl->maxlvl[ZLOG_DEST_FILE] = level;
    }

  return 1;
}
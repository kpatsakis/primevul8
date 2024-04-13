_zlog_assert_failed (const char *assertion, const char *file,
		     unsigned int line, const char *function)
{
  /* Force fallback file logging? */
  if (zlog_default && !zlog_default->fp &&
      ((logfile_fd = open_crashlog()) >= 0) &&
      ((zlog_default->fp = fdopen(logfile_fd, "w")) != NULL))
    zlog_default->maxlvl[ZLOG_DEST_FILE] = LOG_ERR;
  zlog(NULL, LOG_CRIT, "Assertion `%s' failed in file %s, line %u, function %s",
       assertion,file,line,(function ? function : "?"));
  zlog_backtrace(LOG_CRIT);
  abort();
}
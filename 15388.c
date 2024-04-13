vzlog (struct zlog *zl, int priority, const char *format, va_list args)
{
  struct timestamp_control tsctl;
  tsctl.already_rendered = 0;

  /* If zlog is not specified, use default one. */
  if (zl == NULL)
    zl = zlog_default;

  /* When zlog_default is also NULL, use stderr for logging. */
  if (zl == NULL)
    {
      tsctl.precision = 0;
      time_print(stderr, &tsctl);
      fprintf (stderr, "%s: ", "unknown");
      vfprintf (stderr, format, args);
      fprintf (stderr, "\n");
      fflush (stderr);

      /* In this case we return at here. */
      return;
    }
  tsctl.precision = zl->timestamp_precision;

  /* Syslog output */
  if (priority <= zl->maxlvl[ZLOG_DEST_SYSLOG])
    {
      va_list ac;
      va_copy(ac, args);
      vsyslog (priority|zlog_default->facility, format, ac);
      va_end(ac);
    }

  /* File output. */
  if ((priority <= zl->maxlvl[ZLOG_DEST_FILE]) && zl->fp)
    {
      va_list ac;
      time_print (zl->fp, &tsctl);
      if (zl->record_priority)
	fprintf (zl->fp, "%s: ", zlog_priority[priority]);
      fprintf (zl->fp, "%s: ", zlog_proto_names[zl->protocol]);
      va_copy(ac, args);
      vfprintf (zl->fp, format, ac);
      va_end(ac);
      fprintf (zl->fp, "\n");
      fflush (zl->fp);
    }

  /* stdout output. */
  if (priority <= zl->maxlvl[ZLOG_DEST_STDOUT])
    {
      va_list ac;
      time_print (stdout, &tsctl);
      if (zl->record_priority)
	fprintf (stdout, "%s: ", zlog_priority[priority]);
      fprintf (stdout, "%s: ", zlog_proto_names[zl->protocol]);
      va_copy(ac, args);
      vfprintf (stdout, format, ac);
      va_end(ac);
      fprintf (stdout, "\n");
      fflush (stdout);
    }

  /* Terminal monitor. */
  if (priority <= zl->maxlvl[ZLOG_DEST_MONITOR])
    vty_log ((zl->record_priority ? zlog_priority[priority] : NULL),
	     zlog_proto_names[zl->protocol], format, &tsctl, args);
}
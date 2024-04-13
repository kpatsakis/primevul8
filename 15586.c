openzlog (const char *progname, zlog_proto_t protocol,
	  int syslog_flags, int syslog_facility)
{
  struct zlog *zl;
  u_int i;

  zl = XCALLOC(MTYPE_ZLOG, sizeof (struct zlog));

  zl->ident = progname;
  zl->protocol = protocol;
  zl->facility = syslog_facility;
  zl->syslog_options = syslog_flags;

  /* Set default logging levels. */
  for (i = 0; i < sizeof(zl->maxlvl)/sizeof(zl->maxlvl[0]); i++)
    zl->maxlvl[i] = ZLOG_DISABLED;
  zl->maxlvl[ZLOG_DEST_MONITOR] = LOG_DEBUG;
  zl->default_lvl = LOG_DEBUG;

  openlog (progname, syslog_flags, zl->facility);
  
  return zl;
}
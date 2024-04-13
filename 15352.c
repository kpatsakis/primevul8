zlog_signal(int signo, const char *action
#ifdef SA_SIGINFO
	    , siginfo_t *siginfo, void *program_counter
#endif
	   )
{
  time_t now;
  char buf[sizeof("DEFAULT: Received signal S at T (si_addr 0xP, PC 0xP); aborting...")+100];
  char *s = buf;
  char *msgstart = buf;
#define LOC s,buf+sizeof(buf)-s

  time(&now);
  if (zlog_default)
    {
      s = str_append(LOC,zlog_proto_names[zlog_default->protocol]);
      *s++ = ':';
      *s++ = ' ';
      msgstart = s;
    }
  s = str_append(LOC,"Received signal ");
  s = num_append(LOC,signo);
  s = str_append(LOC," at ");
  s = num_append(LOC,now);
#ifdef SA_SIGINFO
  s = str_append(LOC," (si_addr 0x");
  s = hex_append(LOC,(u_long)(siginfo->si_addr));
  if (program_counter)
    {
      s = str_append(LOC,", PC 0x");
      s = hex_append(LOC,(u_long)program_counter);
    }
  s = str_append(LOC,"); ");
#else /* SA_SIGINFO */
  s = str_append(LOC,"; ");
#endif /* SA_SIGINFO */
  s = str_append(LOC,action);
  if (s < buf+sizeof(buf))
    *s++ = '\n';

  /* N.B. implicit priority is most severe */
#define PRI LOG_CRIT

#define DUMP(FD) write(FD, buf, s-buf);
  /* If no file logging configured, try to write to fallback log file. */
  if ((logfile_fd >= 0) || ((logfile_fd = open_crashlog()) >= 0))
    DUMP(logfile_fd)
  if (!zlog_default)
    DUMP(STDERR_FILENO)
  else
    {
      if (PRI <= zlog_default->maxlvl[ZLOG_DEST_STDOUT])
        DUMP(STDOUT_FILENO)
      /* Remove trailing '\n' for monitor and syslog */
      *--s = '\0';
      if (PRI <= zlog_default->maxlvl[ZLOG_DEST_MONITOR])
        vty_log_fixed(buf,s-buf);
      if (PRI <= zlog_default->maxlvl[ZLOG_DEST_SYSLOG])
	syslog_sigsafe(PRI|zlog_default->facility,msgstart,s-msgstart);
    }
#undef DUMP

  zlog_backtrace_sigsafe(PRI,
#ifdef SA_SIGINFO
  			 program_counter
#else
			 NULL
#endif
			);
#undef PRI
#undef LOC
}
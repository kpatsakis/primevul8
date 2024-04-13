log_write(unsigned int selector, int flags, const char *format, ...)
{
uschar * ptr;
int paniclogfd;
ssize_t written_len;
gstring gs = { .size = LOG_BUFFER_SIZE-1, .ptr = 0, .s = log_buffer };
gstring * g;
va_list ap;

/* If panic_recurseflag is set, we have failed to open the panic log. This is
the ultimate disaster. First try to write the message to a debug file and/or
stderr and also to syslog. If panic_save_buffer is not NULL, it contains the
original log line that caused the problem. Afterwards, expire. */

if (panic_recurseflag)
  {
  uschar *extra = panic_save_buffer ? panic_save_buffer : US"";
  if (debug_file) debug_printf("%s%s", extra, log_buffer);
  if (log_stderr && log_stderr != debug_file)
    fprintf(log_stderr, "%s%s", extra, log_buffer);
  if (*extra) write_syslog(LOG_CRIT, extra);
  write_syslog(LOG_CRIT, log_buffer);
  die(US"exim: could not open panic log - aborting: see message(s) above",
    US"Unexpected log failure, please try later");
  }

/* Ensure we have a buffer (see comment above); this should never be obeyed
when running Exim proper, only when running utilities. */

if (!log_buffer)
  if (!(log_buffer = US malloc(LOG_BUFFER_SIZE)))
    {
    fprintf(stderr, "exim: failed to get store for log buffer\n");
    exim_exit(EXIT_FAILURE, NULL);
    }

/* If we haven't already done so, inspect the setting of log_file_path to
determine whether to log to files and/or to syslog. Bits in logging_mode
control this, and for file logging, the path must end up in file_path. This
variable must be in permanent store because it may be required again later in
the process. */

if (!path_inspected)
  {
  BOOL multiple = FALSE;
  int old_pool = store_pool;

  store_pool = POOL_PERM;

  /* If nothing has been set, don't waste effort... the default values for the
  statics are file_path="" and logging_mode = LOG_MODE_FILE. */

  if (*log_file_path)
    {
    int sep = ':';              /* Fixed separator - outside use */
    uschar *s;
    const uschar *ss = log_file_path;

    logging_mode = 0;
    while ((s = string_nextinlist(&ss, &sep, log_buffer, LOG_BUFFER_SIZE)))
      {
      if (Ustrcmp(s, "syslog") == 0)
        logging_mode |= LOG_MODE_SYSLOG;
      else if (logging_mode & LOG_MODE_FILE)
	multiple = TRUE;
      else
        {
        logging_mode |= LOG_MODE_FILE;

        /* If a non-empty path is given, use it */

        if (*s)
          file_path = string_copy(s);

        /* If the path is empty, we want to use the first non-empty, non-
        syslog item in LOG_FILE_PATH, if there is one, since the value of
        log_file_path may have been set at runtime. If there is no such item,
        use the ultimate default in the spool directory. */

        else
	  set_file_path();  /* Empty item in log_file_path */
        }    /* First non-syslog item in log_file_path */
      }      /* Scan of log_file_path */
    }

  /* If no modes have been selected, it is a major disaster */

  if (logging_mode == 0)
    die(US"Neither syslog nor file logging set in log_file_path",
        US"Unexpected logging failure");

  /* Set up the ultimate default if necessary. Then revert to the old store
  pool, and record that we've sorted out the path. */

  if (logging_mode & LOG_MODE_FILE  &&  !file_path[0])
    file_path = string_sprintf("%s/log/%%slog", spool_directory);
  store_pool = old_pool;
  path_inspected = TRUE;

  /* If more than one file path was given, log a complaint. This recursive call
  should work since we have now set up the routing. */

  if (multiple)
    log_write(0, LOG_MAIN|LOG_PANIC,
      "More than one path given in log_file_path: using %s", file_path);
  }

/* If debugging, show all log entries, but don't show headers. Do it all
in one go so that it doesn't get split when multi-processing. */

DEBUG(D_any|D_v)
  {
  int i;

  g = string_catn(&gs, US"LOG:", 4);

  /* Show the selector that was passed into the call. */

  for (i = 0; i < log_options_count; i++)
    {
    unsigned int bitnum = log_options[i].bit;
    if (bitnum < BITWORDSIZE && selector == BIT(bitnum))
      g = string_fmt_append(g, " %s", log_options[i].name);
    }

  g = string_fmt_append(g, "%s%s%s%s\n  ",
    flags & LOG_MAIN ?    " MAIN"   : "",
    flags & LOG_PANIC ?   " PANIC"  : "",
    (flags & LOG_PANIC_DIE) == LOG_PANIC_DIE ? " DIE" : "",
    flags & LOG_REJECT ?  " REJECT" : "");

  if (flags & LOG_CONFIG) g = log_config_info(g, flags);

  va_start(ap, format);
  i = g->ptr;
  if (!string_vformat(g, FALSE, format, ap))
    {
    g->ptr = i;
    g = string_cat(g, US"**** log string overflowed log buffer ****");
    }
  va_end(ap);

  g->size = LOG_BUFFER_SIZE;
  g = string_catn(g, US"\n", 1);
  debug_printf("%s", string_from_gstring(g));

  gs.size = LOG_BUFFER_SIZE-1;	/* Having used the buffer for debug output, */
  gs.ptr = 0;			/* reset it for the real use. */
  gs.s = log_buffer;
  }
/* If no log file is specified, we are in a mess. */

if (!(flags & (LOG_MAIN|LOG_PANIC|LOG_REJECT)))
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "log_write called with no log "
    "flags set");

/* There are some weird circumstances in which logging is disabled. */

if (f.disable_logging)
  {
  DEBUG(D_any) debug_printf("log writing disabled\n");
  return;
  }

/* Handle disabled reject log */

if (!write_rejectlog) flags &= ~LOG_REJECT;

/* Create the main message in the log buffer. Do not include the message id
when called by a utility. */

g = string_fmt_append(&gs, "%s ", tod_stamp(tod_log));

if (LOGGING(pid))
  {
  if (!syslog_pid) pid_position[0] = g->ptr;		/* remember begin … */
  g = string_fmt_append(g, "[%d] ", (int)getpid());
  if (!syslog_pid) pid_position[1] = g->ptr;		/*  … and end+1 of the PID */
  }

if (f.really_exim && message_id[0] != 0)
  g = string_fmt_append(g, "%s ", message_id);

if (flags & LOG_CONFIG)
  g = log_config_info(g, flags);

va_start(ap, format);
  {
  int i = g->ptr;
  if (!string_vformat(g, FALSE, format, ap))
    {
    g->ptr = i;
    g = string_cat(g, US"**** log string overflowed log buffer ****\n");
    }
  }
va_end(ap);

/* Add the raw, unrewritten, sender to the message if required. This is done
this way because it kind of fits with LOG_RECIPIENTS. */

if (   flags & LOG_SENDER
   && g->ptr < LOG_BUFFER_SIZE - 10 - Ustrlen(raw_sender))
  g = string_fmt_append(g, " from <%s>", raw_sender);

/* Add list of recipients to the message if required; the raw list,
before rewriting, was saved in raw_recipients. There may be none, if an ACL
discarded them all. */

if (  flags & LOG_RECIPIENTS
   && g->ptr < LOG_BUFFER_SIZE - 6
   && raw_recipients_count > 0)
  {
  int i;
  g = string_fmt_append(g, " for");
  for (i = 0; i < raw_recipients_count; i++)
    {
    uschar * s = raw_recipients[i];
    if (LOG_BUFFER_SIZE - g->ptr < Ustrlen(s) + 3) break;
    g = string_fmt_append(g, " %s", s);
    }
  }

g = string_catn(g, US"\n", 1);
string_from_gstring(g);

/* Handle loggable errors when running a utility, or when address testing.
Write to log_stderr unless debugging (when it will already have been written),
or unless there is no log_stderr (expn called from daemon, for example). */

if (!f.really_exim || f.log_testing_mode)
  {
  if (  !debug_selector
     && log_stderr
     && (selector == 0 || (selector & log_selector[0]) != 0)
    )
    if (host_checking)
      fprintf(log_stderr, "LOG: %s", CS(log_buffer + 20));  /* no timestamp */
    else
      fprintf(log_stderr, "%s", CS log_buffer);

  if ((flags & LOG_PANIC_DIE) == LOG_PANIC_DIE) exim_exit(EXIT_FAILURE, US"");
  return;
  }

/* Handle the main log. We know that either syslog or file logging (or both) is
set up. A real file gets left open during reception or delivery once it has
been opened, but we don't want to keep on writing to it for too long after it
has been renamed. Therefore, do a stat() and see if the inode has changed, and
if so, re-open. */

if (  flags & LOG_MAIN
   && (!selector ||  selector & log_selector[0]))
  {
  if (  logging_mode & LOG_MODE_SYSLOG
     && (syslog_duplication || !(flags & (LOG_REJECT|LOG_PANIC))))
    write_syslog(LOG_INFO, log_buffer);

  if (logging_mode & LOG_MODE_FILE)
    {
    struct stat statbuf;

    /* Check for a change to the mainlog file name when datestamping is in
    operation. This happens at midnight, at which point we want to roll over
    the file. Closing it has the desired effect. */

    if (mainlog_datestamp)
      {
      uschar *nowstamp = tod_stamp(string_datestamp_type);
      if (Ustrncmp (mainlog_datestamp, nowstamp, Ustrlen(nowstamp)) != 0)
        {
        (void)close(mainlogfd);       /* Close the file */
        mainlogfd = -1;               /* Clear the file descriptor */
        mainlog_inode = 0;            /* Unset the inode */
        mainlog_datestamp = NULL;     /* Clear the datestamp */
        }
      }

    /* Otherwise, we want to check whether the file has been renamed by a
    cycling script. This could be "if else", but for safety's sake, leave it as
    "if" so that renaming the log starts a new file even when datestamping is
    happening. */

    if (mainlogfd >= 0)
      if (Ustat(mainlog_name, &statbuf) < 0 || statbuf.st_ino != mainlog_inode)
	mainlog_close();

    /* If the log is closed, open it. Then write the line. */

    if (mainlogfd < 0)
      {
      open_log(&mainlogfd, lt_main, NULL);     /* No return on error */
      if (fstat(mainlogfd, &statbuf) >= 0) mainlog_inode = statbuf.st_ino;
      }

    /* Failing to write to the log is disastrous */

    written_len = write_to_fd_buf(mainlogfd, g->s, g->ptr);
    if (written_len != g->ptr)
      {
      log_write_failed(US"main log", g->ptr, written_len);
      /* That function does not return */
      }
    }
  }

/* Handle the log for rejected messages. This can be globally disabled, in
which case the flags are altered above. If there are any header lines (i.e. if
the rejection is happening after the DATA phase), log the recipients and the
headers. */

if (flags & LOG_REJECT)
  {
  header_line *h;

  if (header_list && LOGGING(rejected_header))
    {
    uschar * p = g->s + g->ptr;
    int i;

    if (recipients_count > 0)
      {
      /* List the sender */

      string_format(p, LOG_BUFFER_SIZE - g->ptr,
        "Envelope-from: <%s>\n", sender_address);
      while (*p) p++;
      g->ptr = p - g->s;

      /* List up to 5 recipients */

      string_format(p, LOG_BUFFER_SIZE - g->ptr,
        "Envelope-to: <%s>\n", recipients_list[0].address);
      while (*p) p++;
      g->ptr = p - g->s;

      for (i = 1; i < recipients_count && i < 5; i++)
        {
        string_format(p, LOG_BUFFER_SIZE - g->ptr, "    <%s>\n",
          recipients_list[i].address);
	while (*p) p++;
	g->ptr = p - g->s;
        }

      if (i < recipients_count)
        {
        string_format(p, LOG_BUFFER_SIZE - g->ptr,
          "    ...\n");
	while (*p) p++;
	g->ptr = p - g->s;
        }
      }

    /* A header with a NULL text is an unfilled in Received: header */

    for (h = header_list; h; h = h->next) if (h->text)
      {
      BOOL fitted = string_format(p, LOG_BUFFER_SIZE - g->ptr,
        "%c %s", h->type, h->text);
      while (*p) p++;
      g->ptr = p - g->s;
      if (!fitted)         /* Buffer is full; truncate */
        {
        g->ptr -= 100;        /* For message and separator */
        if (g->s[g->ptr-1] == '\n') g->ptr--;
        g = string_cat(g, US"\n*** truncated ***\n");
        break;
        }
      }
    }

  /* Write to syslog or to a log file */

  if (  logging_mode & LOG_MODE_SYSLOG
     && (syslog_duplication || !(flags & LOG_PANIC)))
    write_syslog(LOG_NOTICE, string_from_gstring(g));

  /* Check for a change to the rejectlog file name when datestamping is in
  operation. This happens at midnight, at which point we want to roll over
  the file. Closing it has the desired effect. */

  if (logging_mode & LOG_MODE_FILE)
    {
    struct stat statbuf;

    if (rejectlog_datestamp)
      {
      uschar *nowstamp = tod_stamp(string_datestamp_type);
      if (Ustrncmp (rejectlog_datestamp, nowstamp, Ustrlen(nowstamp)) != 0)
        {
        (void)close(rejectlogfd);       /* Close the file */
        rejectlogfd = -1;               /* Clear the file descriptor */
        rejectlog_inode = 0;            /* Unset the inode */
        rejectlog_datestamp = NULL;     /* Clear the datestamp */
        }
      }

    /* Otherwise, we want to check whether the file has been renamed by a
    cycling script. This could be "if else", but for safety's sake, leave it as
    "if" so that renaming the log starts a new file even when datestamping is
    happening. */

    if (rejectlogfd >= 0)
      if (Ustat(rejectlog_name, &statbuf) < 0 ||
           statbuf.st_ino != rejectlog_inode)
        {
        (void)close(rejectlogfd);
        rejectlogfd = -1;
        rejectlog_inode = 0;
        }

    /* Open the file if necessary, and write the data */

    if (rejectlogfd < 0)
      {
      open_log(&rejectlogfd, lt_reject, NULL); /* No return on error */
      if (fstat(rejectlogfd, &statbuf) >= 0) rejectlog_inode = statbuf.st_ino;
      }

    written_len = write_to_fd_buf(rejectlogfd, g->s, g->ptr);
    if (written_len != g->ptr)
      {
      log_write_failed(US"reject log", g->ptr, written_len);
      /* That function does not return */
      }
    }
  }


/* Handle the panic log, which is not kept open like the others. If it fails to
open, there will be a recursive call to log_write(). We detect this above and
attempt to write to the system log as a last-ditch try at telling somebody. In
all cases except mua_wrapper, try to write to log_stderr. */

if (flags & LOG_PANIC)
  {
  if (log_stderr && log_stderr != debug_file && !mua_wrapper)
    fprintf(log_stderr, "%s", CS string_from_gstring(g));

  if (logging_mode & LOG_MODE_SYSLOG)
    write_syslog(LOG_ALERT, log_buffer);

  /* If this panic logging was caused by a failure to open the main log,
  the original log line is in panic_save_buffer. Make an attempt to write it. */

  if (logging_mode & LOG_MODE_FILE)
    {
    panic_recurseflag = TRUE;
    open_log(&paniclogfd, lt_panic, NULL);  /* Won't return on failure */
    panic_recurseflag = FALSE;

    if (panic_save_buffer)
      {
      int i = write(paniclogfd, panic_save_buffer, Ustrlen(panic_save_buffer));
      i = i;	/* compiler quietening */
      }

    written_len = write_to_fd_buf(paniclogfd, g->s, g->ptr);
    if (written_len != g->ptr)
      {
      int save_errno = errno;
      write_syslog(LOG_CRIT, log_buffer);
      sprintf(CS log_buffer, "write failed on panic log: length=%d result=%d "
        "errno=%d (%s)", g->ptr, (int)written_len, save_errno, strerror(save_errno));
      write_syslog(LOG_CRIT, string_from_gstring(g));
      flags |= LOG_PANIC_DIE;
      }

    (void)close(paniclogfd);
    }

  /* Give up if the DIE flag is set */

  if ((flags & LOG_PANIC_DIE) != LOG_PANIC)
    die(NULL, US"Unexpected failure, please try later");
  }
}
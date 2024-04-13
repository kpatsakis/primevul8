debug_vprintf(int indent, const char *format, va_list ap)
{
int save_errno = errno;

if (!debug_file) return;

/* Various things can be inserted at the start of a line. Don't use the
tod_stamp() function for the timestamp, because that will overwrite the
timestamp buffer, which may contain something useful. (This was a bug fix: the
+memory debugging with +timestamp did cause a problem.) */

if (debug_ptr == debug_buffer)
  {
  DEBUG(D_timestamp)
    {
    struct timeval now;
    time_t tmp;
    struct tm * t;

    gettimeofday(&now, NULL);
    tmp = now.tv_sec;
    t = f.timestamps_utc ? gmtime(&tmp) : localtime(&tmp);
    debug_ptr += sprintf(CS debug_ptr,
      LOGGING(millisec) ? "%02d:%02d:%02d.%03d " : "%02d:%02d:%02d ",
      t->tm_hour, t->tm_min, t->tm_sec, (int)(now.tv_usec/1000));
    }

  DEBUG(D_pid)
    debug_ptr += sprintf(CS debug_ptr, "%5d ", (int)getpid());

  /* Set up prefix if outputting for host checking and not debugging */

  if (host_checking && debug_selector == 0)
    {
    Ustrcpy(debug_ptr, ">>> ");
    debug_ptr += 4;
    }

  debug_prefix_length = debug_ptr - debug_buffer;
  }

if (indent > 0)
  {
  int i;
  for (i = indent >> 2; i > 0; i--)
    DEBUG(D_noutf8)
      {
      Ustrcpy(debug_ptr, "   !");
      debug_ptr += 4;	/* 3 spaces + shriek */
      debug_prefix_length += 4;
      }
    else
      {
      Ustrcpy(debug_ptr, "   " UTF8_VERT_2DASH);
      debug_ptr += 6;	/* 3 spaces + 3 UTF-8 octets */
      debug_prefix_length += 6;
      }

  Ustrncpy(debug_ptr, "   ", indent &= 3);
  debug_ptr += indent;
  debug_prefix_length += indent;
  }

/* Use the checked formatting routine to ensure that the buffer
does not overflow. Ensure there's space for a newline at the end. */

  {
  gstring gs = { .size = (int)sizeof(debug_buffer) - 1,
		.ptr = debug_ptr - debug_buffer,
		.s = debug_buffer };
  if (!string_vformat(&gs, FALSE, format, ap))
    {
    uschar * s = US"**** debug string too long - truncated ****\n";
    uschar * p = gs.s + gs.ptr;
    int maxlen = gs.size - Ustrlen(s) - 2;
    if (p > gs.s + maxlen) p = gs.s + maxlen;
    if (p > gs.s && p[-1] != '\n') *p++ = '\n';
    Ustrcpy(p, s);
    while(*debug_ptr) debug_ptr++;
    }
  else
    {
    string_from_gstring(&gs);
    debug_ptr = gs.s + gs.ptr;
    }
  }

/* Output the line if it is complete. If we added any prefix data and there
are internal newlines, make sure the prefix is on the continuation lines,
as long as there is room in the buffer. We want to do just a single fprintf()
so as to avoid interleaving. */

if (debug_ptr[-1] == '\n')
  {
  if (debug_prefix_length > 0)
    {
    uschar *p = debug_buffer;
    int left = sizeof(debug_buffer) - (debug_ptr - debug_buffer) - 1;
    while ((p = Ustrchr(p, '\n') + 1) != debug_ptr &&
           left >= debug_prefix_length)
      {
      int len = debug_ptr - p;
      memmove(p + debug_prefix_length, p, len + 1);
      memmove(p, debug_buffer, debug_prefix_length);
      debug_ptr += debug_prefix_length;
      left -= debug_prefix_length;
      }
    }

  fprintf(debug_file, "%s", CS debug_buffer);
  fflush(debug_file);
  debug_ptr = debug_buffer;
  debug_prefix_length = 0;
  }
errno = save_errno;
}
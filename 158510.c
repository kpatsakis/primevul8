lmtp_read_response(FILE *f, uschar *buffer, int size, int okdigit, int timeout)
{
int count;
uschar *ptr = buffer;
uschar *readptr = buffer;

/* Ensure errno starts out zero */

errno = 0;

/* Loop for handling LMTP responses that do not all come in one line. */

for (;;)
  {
  /* If buffer is too full, something has gone wrong. */

  if (size < 10)
    {
    *readptr = 0;
    errno = ERRNO_SMTPFORMAT;
    return FALSE;
    }

  /* Loop to cover the read getting interrupted. */

  for (;;)
    {
    char *rc;
    int save_errno;

    *readptr = 0;           /* In case nothing gets read */
    sigalrm_seen = FALSE;
    ALARM(timeout);
    rc = Ufgets(readptr, size-1, f);
    save_errno = errno;
    ALARM_CLR(0);
    errno = save_errno;

    if (rc != NULL) break;  /* A line has been read */

    /* Handle timeout; must do this first because it uses EINTR */

    if (sigalrm_seen) errno = ETIMEDOUT;

    /* If some other interrupt arrived, just retry. We presume this to be rare,
    but it can happen (e.g. the SIGUSR1 signal sent by exiwhat causes
    read() to exit). */

    else if (errno == EINTR)
      {
      DEBUG(D_transport) debug_printf("EINTR while reading LMTP response\n");
      continue;
      }

    /* Handle other errors, including EOF; ensure buffer is completely empty. */

    buffer[0] = 0;
    return FALSE;
    }

  /* Adjust size in case we have to read another line, and adjust the
  count to be the length of the line we are about to inspect. */

  count = Ustrlen(readptr);
  size -= count;
  count += readptr - ptr;

  /* See if the final two characters in the buffer are \r\n. If not, we
  have to read some more. At least, that is what we should do on a strict
  interpretation of the RFC. But accept LF as well, as we do for SMTP. */

  if (ptr[count-1] != '\n')
    {
    DEBUG(D_transport)
      {
      int i;
      debug_printf("LMTP input line incomplete in one buffer:\n  ");
      for (i = 0; i < count; i++)
        {
        int c = (ptr[i]);
        if (mac_isprint(c)) debug_printf("%c", c); else debug_printf("<%d>", c);
        }
      debug_printf("\n");
      }
    readptr = ptr + count;
    continue;
    }

  /* Remove any whitespace at the end of the buffer. This gets rid of CR, LF
  etc. at the end. Show it, if debugging, formatting multi-line responses. */

  while (count > 0 && isspace(ptr[count-1])) count--;
  ptr[count] = 0;

  DEBUG(D_transport|D_v)
    {
    uschar *s = ptr;
    uschar *t = ptr;
    while (*t != 0)
      {
      while (*t != 0 && *t != '\n') t++;
      debug_printf("  %s %*s\n", (s == ptr)? "LMTP<<" : "      ",
        (int)(t-s), s);
      if (*t == 0) break;
      s = t = t + 1;
      }
    }

  /* Check the format of the response: it must start with three digits; if
  these are followed by a space or end of line, the response is complete. If
  they are followed by '-' this is a multi-line response and we must look for
  another line until the final line is reached. The only use made of multi-line
  responses is to pass them back as error messages. We therefore just
  concatenate them all within the buffer, which should be large enough to
  accept any reasonable number of lines. A multiline response may already
  have been read in one go - hence the loop here. */

  for(;;)
    {
    uschar *p;
    if (count < 3 ||
       !isdigit(ptr[0]) ||
       !isdigit(ptr[1]) ||
       !isdigit(ptr[2]) ||
       (ptr[3] != '-' && ptr[3] != ' ' && ptr[3] != 0))
      {
      errno = ERRNO_SMTPFORMAT;    /* format error */
      return FALSE;
      }

    /* If a single-line response, exit the loop */

    if (ptr[3] != '-') break;

    /* For a multi-line response see if the next line is already read, and if
    so, stay in this loop to check it. */

    p = ptr + 3;
    while (*(++p) != 0)
      {
      if (*p == '\n')
        {
        ptr = ++p;
        break;
        }
      }
    if (*p == 0) break;   /* No more lines to check */
    }

  /* End of response. If the last of the lines we are looking at is the final
  line, we are done. Otherwise more data has to be read. */

  if (ptr[3] != '-') break;

  /* Move the reading pointer upwards in the buffer and insert \n in case this
  is an error message that subsequently gets printed. Set the scanning pointer
  to the reading pointer position. */

  ptr += count;
  *ptr++ = '\n';
  size--;
  readptr = ptr;
  }

/* Return a value that depends on the LMTP return code. Ensure that errno is
zero, because the caller of this function looks at errno when FALSE is
returned, to distinguish between an unexpected return code and other errors
such as timeouts, lost connections, etc. */

errno = 0;
return buffer[0] == okdigit;
}
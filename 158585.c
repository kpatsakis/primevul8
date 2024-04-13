read_response_line(smtp_inblock *inblock, uschar *buffer, int size, int timeout)
{
uschar *p = buffer;
uschar *ptr = inblock->ptr;
uschar *ptrend = inblock->ptrend;
client_conn_ctx * cctx = inblock->cctx;

/* Loop for reading multiple packets or reading another packet after emptying
a previously-read one. */

for (;;)
  {
  int rc;

  /* If there is data in the input buffer left over from last time, copy
  characters from it until the end of a line, at which point we can return,
  having removed any whitespace (which will include CR) at the end of the line.
  The rules for SMTP say that lines end in CRLF, but there are have been cases
  of hosts using just LF, and other MTAs are reported to handle this, so we
  just look for LF. If we run out of characters before the end of a line,
  carry on to read the next incoming packet. */

  while (ptr < ptrend)
    {
    int c = *ptr++;
    if (c == '\n')
      {
      while (p > buffer && isspace(p[-1])) p--;
      *p = 0;
      inblock->ptr = ptr;
      return p - buffer;
      }
    *p++ = c;
    if (--size < 4)
      {
      *p = 0;                     /* Leave malformed line for error message */
      errno = ERRNO_SMTPFORMAT;
      return -1;
      }
    }

  /* Need to read a new input packet. */

  if((rc = ip_recv(cctx, inblock->buffer, inblock->buffersize, timeout)) <= 0)
    {
    DEBUG(D_deliver|D_transport|D_acl)
      debug_printf_indent(errno ? "  SMTP(%s)<<\n" : "  SMTP(closed)<<\n",
	strerror(errno));
    break;
    }

  /* Another block of data has been successfully read. Set up the pointers
  and let the loop continue. */

  ptrend = inblock->ptrend = inblock->buffer + rc;
  ptr = inblock->buffer;
  DEBUG(D_transport|D_acl) debug_printf_indent("read response data: size=%d\n", rc);
  }

/* Get here if there has been some kind of recv() error; errno is set, but we
ensure that the result buffer is empty before returning. */

*buffer = 0;
return -1;
}
smtp_read_response(void * sx0, uschar *buffer, int size, int okdigit,
   int timeout)
{
smtp_context * sx = sx0;
uschar *ptr = buffer;
int count = 0;

errno = 0;  /* Ensure errno starts out zero */

#ifdef EXPERIMENTAL_PIPE_CONNECT
if (sx->pending_BANNER || sx->pending_EHLO)
  if (smtp_reap_early_pipe(sx, &count) != OK)
    {
    DEBUG(D_transport) debug_printf("failed reaping pipelined cmd responsess\n");
    return FALSE;
    }
#endif

/* This is a loop to read and concatenate the lines that make up a multi-line
response. */

for (;;)
  {
  if ((count = read_response_line(&sx->inblock, ptr, size, timeout)) < 0)
    return FALSE;

  HDEBUG(D_transport|D_acl|D_v)
    debug_printf_indent("  %s %s\n", ptr == buffer ? "SMTP<<" : "      ", ptr);

  /* Check the format of the response: it must start with three digits; if
  these are followed by a space or end of line, the response is complete. If
  they are followed by '-' this is a multi-line response and we must look for
  another line until the final line is reached. The only use made of multi-line
  responses is to pass them back as error messages. We therefore just
  concatenate them all within the buffer, which should be large enough to
  accept any reasonable number of lines. */

  if (count < 3 ||
     !isdigit(ptr[0]) ||
     !isdigit(ptr[1]) ||
     !isdigit(ptr[2]) ||
     (ptr[3] != '-' && ptr[3] != ' ' && ptr[3] != 0))
    {
    errno = ERRNO_SMTPFORMAT;    /* format error */
    return FALSE;
    }

  /* If the line we have just read is a terminal line, line, we are done.
  Otherwise more data has to be read. */

  if (ptr[3] != '-') break;

  /* Move the reading pointer upwards in the buffer and insert \n between the
  components of a multiline response. Space is left for this by read_response_
  line(). */

  ptr += count;
  *ptr++ = '\n';
  size -= count + 1;
  }

#ifdef TCP_FASTOPEN
  tfo_out_check(sx->cctx.sock);
#endif

/* Return a value that depends on the SMTP return code. On some systems a
non-zero value of errno has been seen at this point, so ensure it is zero,
because the caller of this function looks at errno when FALSE is returned, to
distinguish between an unexpected return code and other errors such as
timeouts, lost connections, etc. */

errno = 0;
return buffer[0] == okdigit;
}
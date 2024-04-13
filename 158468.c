wouldblock_reading(void)
{
int fd, rc;
fd_set fds;
struct timeval tzero;

#ifdef SUPPORT_TLS
if (tls_in.active.sock >= 0)
 return !tls_could_read();
#endif

if (smtp_inptr < smtp_inend)
  return FALSE;

fd = fileno(smtp_in);
FD_ZERO(&fds);
FD_SET(fd, &fds);
tzero.tv_sec = 0;
tzero.tv_usec = 0;
rc = select(fd + 1, (SELECT_ARG2_TYPE *)&fds, NULL, NULL, &tzero);

if (rc <= 0) return TRUE;     /* Not ready to read */
rc = smtp_getc(GETC_BUFFER_UNLIMITED);
if (rc < 0) return TRUE;      /* End of file or error */

smtp_ungetc(rc);
return FALSE;
}
transport_write_block_fd(transport_ctx * tctx, uschar *block, int len, BOOL more)
{
int i, rc, save_errno;
int local_timeout = transport_write_timeout;
int fd = tctx->u.fd;

/* This loop is for handling incomplete writes and other retries. In most
normal cases, it is only ever executed once. */

for (i = 0; i < 100; i++)
  {
  DEBUG(D_transport)
    debug_printf("writing data block fd=%d size=%d timeout=%d%s\n",
      fd, len, local_timeout, more ? " (more expected)" : "");

  /* This code makes use of alarm() in order to implement the timeout. This
  isn't a very tidy way of doing things. Using non-blocking I/O with select()
  provides a neater approach. However, I don't know how to do this when TLS is
  in use. */

  if (transport_write_timeout <= 0)   /* No timeout wanted */
    {
    rc =
#ifdef SUPPORT_TLS
	tls_out.active.sock == fd ? tls_write(tls_out.active.tls_ctx, block, len, more) :
#endif
#ifdef MSG_MORE
	more && !(tctx->options & topt_not_socket)
	  ? send(fd, block, len, MSG_MORE) :
#endif
	write(fd, block, len);
    save_errno = errno;
    }

  /* Timeout wanted. */

  else
    {
    ALARM(local_timeout);

    rc =
#ifdef SUPPORT_TLS
	tls_out.active.sock == fd ? tls_write(tls_out.active.tls_ctx, block, len, more) :
#endif
#ifdef MSG_MORE
	more && !(tctx->options & topt_not_socket)
	  ? send(fd, block, len, MSG_MORE) :
#endif
	write(fd, block, len);

    save_errno = errno;
    local_timeout = ALARM_CLR(0);
    if (sigalrm_seen)
      {
      errno = ETIMEDOUT;
      return FALSE;
      }
    }

  /* Hopefully, the most common case is success, so test that first. */

  if (rc == len) { transport_count += len; return TRUE; }

  /* A non-negative return code is an incomplete write. Try again for the rest
  of the block. If we have exactly hit the timeout, give up. */

  if (rc >= 0)
    {
    len -= rc;
    block += rc;
    transport_count += rc;
    DEBUG(D_transport) debug_printf("write incomplete (%d)\n", rc);
    goto CHECK_TIMEOUT;   /* A few lines below */
    }

  /* A negative return code with an EINTR error is another form of
  incomplete write, zero bytes having been written */

  if (save_errno == EINTR)
    {
    DEBUG(D_transport)
      debug_printf("write interrupted before anything written\n");
    goto CHECK_TIMEOUT;   /* A few lines below */
    }

  /* A response of EAGAIN from write() is likely only in the case of writing
  to a FIFO that is not swallowing the data as fast as Exim is writing it. */

  if (save_errno == EAGAIN)
    {
    DEBUG(D_transport)
      debug_printf("write temporarily locked out, waiting 1 sec\n");
    sleep(1);

    /* Before continuing to try another write, check that we haven't run out of
    time. */

    CHECK_TIMEOUT:
    if (transport_write_timeout > 0 && local_timeout <= 0)
      {
      errno = ETIMEDOUT;
      return FALSE;
      }
    continue;
    }

  /* Otherwise there's been an error */

  DEBUG(D_transport) debug_printf("writing error %d: %s\n", save_errno,
    strerror(save_errno));
  errno = save_errno;
  return FALSE;
  }

/* We've tried and tried and tried but still failed */

errno = ERRNO_WRITEINCOMPLETE;
return FALSE;
}
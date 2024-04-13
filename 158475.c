transport_write_message(transport_ctx * tctx, int size_limit)
{
BOOL last_filter_was_NL = TRUE;
BOOL save_spool_file_wireformat = f.spool_file_wireformat;
int rc, len, yield, fd_read, fd_write, save_errno;
int pfd[2] = {-1, -1};
pid_t filter_pid, write_pid;

f.transport_filter_timed_out = FALSE;

/* If there is no filter command set up, call the internal function that does
the actual work, passing it the incoming fd, and return its result. */

if (  !transport_filter_argv
   || !*transport_filter_argv
   || !**transport_filter_argv
   )
  return internal_transport_write_message(tctx, size_limit);

/* Otherwise the message must be written to a filter process and read back
before being written to the incoming fd. First set up the special processing to
be done during the copying. */

nl_partial_match = -1;

if (tctx->check_string && tctx->escape_string)
  {
  nl_check = tctx->check_string;
  nl_check_length = Ustrlen(nl_check);
  nl_escape = tctx->escape_string;
  nl_escape_length = Ustrlen(nl_escape);
  }
else nl_check_length = nl_escape_length = 0;

/* Start up a subprocess to run the command. Ensure that our main fd will
be closed when the subprocess execs, but remove the flag afterwards.
(Otherwise, if this is a TCP/IP socket, it can't get passed on to another
process to deliver another message.) We get back stdin/stdout file descriptors.
If the process creation failed, give an error return. */

fd_read = -1;
fd_write = -1;
save_errno = 0;
yield = FALSE;
write_pid = (pid_t)(-1);

  {
  int bits = fcntl(tctx->u.fd, F_GETFD);
  (void)fcntl(tctx->u.fd, F_SETFD, bits | FD_CLOEXEC);
  filter_pid = child_open(USS transport_filter_argv, NULL, 077,
   &fd_write, &fd_read, FALSE);
  (void)fcntl(tctx->u.fd, F_SETFD, bits & ~FD_CLOEXEC);
  }
if (filter_pid < 0) goto TIDY_UP;      /* errno set */

DEBUG(D_transport)
  debug_printf("process %d running as transport filter: fd_write=%d fd_read=%d\n",
    (int)filter_pid, fd_write, fd_read);

/* Fork subprocess to write the message to the filter, and return the result
via a(nother) pipe. While writing to the filter, we do not do the CRLF,
smtp dots, or check string processing. */

if (pipe(pfd) != 0) goto TIDY_UP;      /* errno set */
if ((write_pid = fork()) == 0)
  {
  BOOL rc;
  (void)close(fd_read);
  (void)close(pfd[pipe_read]);
  nl_check_length = nl_escape_length = 0;

  tctx->u.fd = fd_write;
  tctx->check_string = tctx->escape_string = NULL;
  tctx->options &= ~(topt_use_crlf | topt_end_dot | topt_use_bdat);

  rc = internal_transport_write_message(tctx, size_limit);

  save_errno = errno;
  if (  write(pfd[pipe_write], (void *)&rc, sizeof(BOOL))
        != sizeof(BOOL)
     || write(pfd[pipe_write], (void *)&save_errno, sizeof(int))
        != sizeof(int)
     || write(pfd[pipe_write], (void *)&tctx->addr->more_errno, sizeof(int))
        != sizeof(int)
     || write(pfd[pipe_write], (void *)&tctx->addr->delivery_usec, sizeof(int))
        != sizeof(int)
     )
    rc = FALSE;	/* compiler quietening */
  _exit(0);
  }
save_errno = errno;

/* Parent process: close our copy of the writing subprocess' pipes. */

(void)close(pfd[pipe_write]);
(void)close(fd_write);
fd_write = -1;

/* Writing process creation failed */

if (write_pid < 0)
  {
  errno = save_errno;    /* restore */
  goto TIDY_UP;
  }

/* When testing, let the subprocess get going */

if (f.running_in_test_harness) millisleep(250);

DEBUG(D_transport)
  debug_printf("process %d writing to transport filter\n", (int)write_pid);

/* Copy the message from the filter to the output fd. A read error leaves len
== -1 and errno set. We need to apply a timeout to the read, to cope with
the case when the filter gets stuck, but it can be quite a long one. The
default is 5m, but this is now configurable. */

DEBUG(D_transport) debug_printf("copying from the filter\n");

/* Copy the output of the filter, remembering if the last character was NL. If
no data is returned, that counts as "ended with NL" (default setting of the
variable is TRUE).  The output should always be unix-format as we converted
any wireformat source on writing input to the filter. */

f.spool_file_wireformat = FALSE;
chunk_ptr = deliver_out_buffer;

for (;;)
  {
  sigalrm_seen = FALSE;
  ALARM(transport_filter_timeout);
  len = read(fd_read, deliver_in_buffer, DELIVER_IN_BUFFER_SIZE);
  ALARM_CLR(0);
  if (sigalrm_seen)
    {
    errno = ETIMEDOUT;
    f.transport_filter_timed_out = TRUE;
    goto TIDY_UP;
    }

  /* If the read was successful, write the block down the original fd,
  remembering whether it ends in \n or not. */

  if (len > 0)
    {
    if (!write_chunk(tctx, deliver_in_buffer, len)) goto TIDY_UP;
    last_filter_was_NL = (deliver_in_buffer[len-1] == '\n');
    }

  /* Otherwise, break the loop. If we have hit EOF, set yield = TRUE. */

  else
    {
    if (len == 0) yield = TRUE;
    break;
    }
  }

/* Tidying up code. If yield = FALSE there has been an error and errno is set
to something. Ensure the pipes are all closed and the processes are removed. If
there has been an error, kill the processes before waiting for them, just to be
sure. Also apply a paranoia timeout. */

TIDY_UP:
f.spool_file_wireformat = save_spool_file_wireformat;
save_errno = errno;

(void)close(fd_read);
if (fd_write > 0) (void)close(fd_write);

if (!yield)
  {
  if (filter_pid > 0) kill(filter_pid, SIGKILL);
  if (write_pid > 0)  kill(write_pid, SIGKILL);
  }

/* Wait for the filter process to complete. */

DEBUG(D_transport) debug_printf("waiting for filter process\n");
if (filter_pid > 0 && (rc = child_close(filter_pid, 30)) != 0 && yield)
  {
  yield = FALSE;
  save_errno = ERRNO_FILTER_FAIL;
  tctx->addr->more_errno = rc;
  DEBUG(D_transport) debug_printf("filter process returned %d\n", rc);
  }

/* Wait for the writing process to complete. If it ends successfully,
read the results from its pipe, provided we haven't already had a filter
process failure. */

DEBUG(D_transport) debug_printf("waiting for writing process\n");
if (write_pid > 0)
  {
  rc = child_close(write_pid, 30);
  if (yield)
    if (rc == 0)
      {
      BOOL ok;
      if (read(pfd[pipe_read], (void *)&ok, sizeof(BOOL)) != sizeof(BOOL))
	{
	DEBUG(D_transport)
	  debug_printf("pipe read from writing process: %s\n", strerror(errno));
	save_errno = ERRNO_FILTER_FAIL;
        yield = FALSE;
	}
      else if (!ok)
        {
	int dummy = read(pfd[pipe_read], (void *)&save_errno, sizeof(int));
        dummy = read(pfd[pipe_read], (void *)&tctx->addr->more_errno, sizeof(int));
        dummy = read(pfd[pipe_read], (void *)&tctx->addr->delivery_usec, sizeof(int));
	dummy = dummy;		/* compiler quietening */
        yield = FALSE;
        }
      }
    else
      {
      yield = FALSE;
      save_errno = ERRNO_FILTER_FAIL;
      tctx->addr->more_errno = rc;
      DEBUG(D_transport) debug_printf("writing process returned %d\n", rc);
      }
  }
(void)close(pfd[pipe_read]);

/* If there have been no problems we can now add the terminating "." if this is
SMTP output, turning off escaping beforehand. If the last character from the
filter was not NL, insert a NL to make the SMTP protocol work. */

if (yield)
  {
  nl_check_length = nl_escape_length = 0;
  f.spool_file_wireformat = FALSE;
  if (  tctx->options & topt_end_dot
     && ( last_filter_was_NL
        ? !write_chunk(tctx, US".\n", 2)
	: !write_chunk(tctx, US"\n.\n", 3)
     )  )
    yield = FALSE;

  /* Write out any remaining data in the buffer. */

  else
    yield = (len = chunk_ptr - deliver_out_buffer) <= 0
	  || transport_write_block(tctx, deliver_out_buffer, len, FALSE);
  }
else
  errno = save_errno;      /* From some earlier error */

DEBUG(D_transport)
  {
  debug_printf("end of filtering transport writing: yield=%d\n", yield);
  if (!yield)
    debug_printf("errno=%d more_errno=%d\n", errno, tctx->addr->more_errno);
  }

return yield;
}
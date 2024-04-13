transport_do_pass_socket(const uschar *transport_name, const uschar *hostname,
  const uschar *hostaddress, uschar *id, int socket_fd)
{
int i = 20;
const uschar **argv;

/* Set up the calling arguments; use the standard function for the basics,
but we have a number of extras that may be added. */

argv = CUSS child_exec_exim(CEE_RETURN_ARGV, TRUE, &i, FALSE, 0);

if (f.smtp_authenticated)			argv[i++] = US"-MCA";
if (smtp_peer_options & OPTION_CHUNKING)	argv[i++] = US"-MCK";
if (smtp_peer_options & OPTION_DSN)		argv[i++] = US"-MCD";
if (smtp_peer_options & OPTION_PIPE)		argv[i++] = US"-MCP";
if (smtp_peer_options & OPTION_SIZE)		argv[i++] = US"-MCS";
#ifdef SUPPORT_TLS
if (smtp_peer_options & OPTION_TLS)
  if (tls_out.active.sock >= 0 || continue_proxy_cipher)
    {
    argv[i++] = US"-MCt";
    argv[i++] = sending_ip_address;
    argv[i++] = string_sprintf("%d", sending_port);
    argv[i++] = tls_out.active.sock >= 0 ? tls_out.cipher : continue_proxy_cipher;
    }
  else
    argv[i++] = US"-MCT";
#endif

if (queue_run_pid != (pid_t)0)
  {
  argv[i++] = US"-MCQ";
  argv[i++] = string_sprintf("%d", queue_run_pid);
  argv[i++] = string_sprintf("%d", queue_run_pipe);
  }

argv[i++] = US"-MC";
argv[i++] = US transport_name;
argv[i++] = US hostname;
argv[i++] = US hostaddress;
argv[i++] = string_sprintf("%d", continue_sequence + 1);
argv[i++] = id;
argv[i++] = NULL;

/* Arrange for the channel to be on stdin. */

if (socket_fd != 0)
  {
  (void)dup2(socket_fd, 0);
  (void)close(socket_fd);
  }

DEBUG(D_exec) debug_print_argv(argv);
exim_nullstd();                          /* Ensure std{out,err} exist */
execv(CS argv[0], (char *const *)argv);

DEBUG(D_any) debug_printf("execv failed: %s\n", strerror(errno));
_exit(errno);         /* Note: must be _exit(), NOT exit() */
}
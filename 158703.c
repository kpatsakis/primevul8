transport_pass_socket(const uschar *transport_name, const uschar *hostname,
  const uschar *hostaddress, uschar *id, int socket_fd)
{
pid_t pid;
int status;

DEBUG(D_transport) debug_printf("transport_pass_socket entered\n");

if ((pid = fork()) == 0)
  {
  /* Disconnect entirely from the parent process. If we are running in the
  test harness, wait for a bit to allow the previous process time to finish,
  write the log, etc., so that the output is always in the same order for
  automatic comparison. */

  if ((pid = fork()) != 0)
    {
    DEBUG(D_transport) debug_printf("transport_pass_socket succeeded (final-pid %d)\n", pid);
    _exit(EXIT_SUCCESS);
    }
  if (f.running_in_test_harness) sleep(1);

  transport_do_pass_socket(transport_name, hostname, hostaddress,
    id, socket_fd);
  }

/* If the process creation succeeded, wait for the first-level child, which
immediately exits, leaving the second level process entirely disconnected from
this one. */

if (pid > 0)
  {
  int rc;
  while ((rc = wait(&status)) != pid && (rc >= 0 || errno != ECHILD));
  DEBUG(D_transport) debug_printf("transport_pass_socket succeeded (inter-pid %d)\n", pid);
  return TRUE;
  }
else
  {
  DEBUG(D_transport) debug_printf("transport_pass_socket failed to fork: %s\n",
    strerror(errno));
  return FALSE;
  }
}
flatpak_run_add_journal_args (GPtrArray *argv_array)
{
  g_autofree char *journal_socket_socket = g_strdup ("/run/systemd/journal/socket");
  g_autofree char *journal_stdout_socket = g_strdup ("/run/systemd/journal/stdout");

  if (g_file_test (journal_socket_socket, G_FILE_TEST_EXISTS))
    {
      add_args (argv_array,
                "--bind", journal_socket_socket, journal_socket_socket,
                NULL);
    }
  if (g_file_test (journal_stdout_socket, G_FILE_TEST_EXISTS))
    {
      add_args (argv_array,
                "--bind", journal_stdout_socket, journal_stdout_socket,
                NULL);
    }
}
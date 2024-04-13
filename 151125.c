flatpak_run_add_pulseaudio_args (GPtrArray *argv_array,
                                 GArray    *fd_array,
                                 char    ***envp_p)
{
  g_autofree char *pulseaudio_socket = g_build_filename (g_get_user_runtime_dir (), "pulse/native", NULL);

  *envp_p = g_environ_unsetenv (*envp_p, "PULSE_SERVER");
  if (g_file_test (pulseaudio_socket, G_FILE_TEST_EXISTS))
    {
      gboolean share_shm = FALSE; /* TODO: When do we add this? */
      g_autofree char *client_config = g_strdup_printf ("enable-shm=%s\n", share_shm ? "yes" : "no");
      g_autofree char *sandbox_socket_path = g_strdup_printf ("/run/user/%d/pulse/native", getuid ());
      g_autofree char *pulse_server = g_strdup_printf ("unix:/run/user/%d/pulse/native", getuid ());
      g_autofree char *config_path = g_strdup_printf ("/run/user/%d/pulse/config", getuid ());
      int fd;
      g_autofree char *fd_str = NULL;

      fd = create_tmp_fd (client_config, -1, NULL);
      if (fd == -1)
        return;

      fd_str = g_strdup_printf ("%d", fd);
      if (fd_array)
        g_array_append_val (fd_array, fd);

      add_args (argv_array,
                "--bind", pulseaudio_socket, sandbox_socket_path,
                "--bind-data", fd_str, config_path,
                NULL);

      *envp_p = g_environ_setenv (*envp_p, "PULSE_SERVER", pulse_server, TRUE);
      *envp_p = g_environ_setenv (*envp_p, "PULSE_CLIENTCONFIG", config_path, TRUE);
    }
}
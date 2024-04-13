flatpak_run_add_x11_args (GPtrArray *argv_array,
                          GArray    *fd_array,
                          char    ***envp_p,
                          gboolean allowed)
{
  g_autofree char *x11_socket = NULL;
  const char *display;

  /* Always cover /tmp/.X11-unix, that way we never see the host one in case
   * we have access to the host /tmp. If you request X access we'll put the right
   * thing in this anyway.
   */
  add_args (argv_array,
            "--tmpfs", "/tmp/.X11-unix",
            NULL);

  if (!allowed)
    {
      *envp_p = g_environ_unsetenv (*envp_p, "DISPLAY");
      return;
    }

  g_debug ("Allowing x11 access");

  display = g_getenv ("DISPLAY");
  if (display && display[0] == ':' && g_ascii_isdigit (display[1]))
    {
      const char *display_nr = &display[1];
      const char *display_nr_end = display_nr;
      g_autofree char *d = NULL;
      g_autofree char *tmp_path = NULL;

      while (g_ascii_isdigit (*display_nr_end))
        display_nr_end++;

      d = g_strndup (display_nr, display_nr_end - display_nr);
      x11_socket = g_strdup_printf ("/tmp/.X11-unix/X%s", d);

      add_args (argv_array,
                "--bind", x11_socket, "/tmp/.X11-unix/X99",
                NULL);
      *envp_p = g_environ_setenv (*envp_p, "DISPLAY", ":99.0", TRUE);

#ifdef ENABLE_XAUTH
      int fd;
      fd = g_file_open_tmp ("flatpak-xauth-XXXXXX", &tmp_path, NULL);
      if (fd >= 0)
        {
          FILE *output = fdopen (fd, "wb");
          if (output != NULL)
            {
              int tmp_fd = dup (fd);
              if (tmp_fd != -1)
                {
                  g_autofree char *tmp_fd_str = g_strdup_printf ("%d", tmp_fd);
                  g_autofree char *dest = g_strdup_printf ("/run/user/%d/Xauthority", getuid ());

                  write_xauth (d, output);
                  add_args (argv_array,
                            "--bind-data", tmp_fd_str, dest,
                            NULL);
                  if (fd_array)
                    g_array_append_val (fd_array, tmp_fd);
                  *envp_p = g_environ_setenv (*envp_p, "XAUTHORITY", dest, TRUE);

                }

              fclose (output);
              unlink (tmp_path);

              if (tmp_fd != -1)
                lseek (tmp_fd, 0, SEEK_SET);
            }
          else
            {
              close (fd);
            }
        }
#endif
    }
  else
    {
      *envp_p = g_environ_unsetenv (*envp_p, "DISPLAY");
    }

}
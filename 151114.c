flatpak_run_add_wayland_args (GPtrArray *argv_array,
                              char    ***envp_p)
{
  g_autofree char *wayland_socket = g_build_filename (g_get_user_runtime_dir (), "wayland-0", NULL);
  g_autofree char *sandbox_wayland_socket = g_strdup_printf ("/run/user/%d/wayland-0", getuid ());

  if (g_file_test (wayland_socket, G_FILE_TEST_EXISTS))
    {
      add_args (argv_array,
                "--bind", wayland_socket, sandbox_wayland_socket,
                NULL);
    }
}
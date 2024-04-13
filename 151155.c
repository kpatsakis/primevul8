flatpak_run_add_system_dbus_args (FlatpakContext *context,
                                  char         ***envp_p,
                                  GPtrArray      *argv_array,
                                  GPtrArray      *dbus_proxy_argv,
                                  gboolean        unrestricted)
{
  const char *dbus_address = g_getenv ("DBUS_SYSTEM_BUS_ADDRESS");
  g_autofree char *real_dbus_address = NULL;
  g_autofree char *dbus_system_socket = NULL;

  if (dbus_address != NULL)
    dbus_system_socket = extract_unix_path_from_dbus_address (dbus_address);
  else if (g_file_test ("/var/run/dbus/system_bus_socket", G_FILE_TEST_EXISTS))
    dbus_system_socket = g_strdup ("/var/run/dbus/system_bus_socket");

  if (dbus_system_socket != NULL && unrestricted)
    {
      add_args (argv_array,
                "--bind", dbus_system_socket, "/run/dbus/system_bus_socket",
                NULL);
      *envp_p = g_environ_setenv (*envp_p, "DBUS_SYSTEM_BUS_ADDRESS", "unix:path=/run/dbus/system_bus_socket", TRUE);

      return TRUE;
    }
  else if (dbus_proxy_argv &&
           g_hash_table_size (context->system_bus_policy) > 0)
    {
      g_autofree char *proxy_socket = create_proxy_socket (".system-bus-proxy-XXXXXX");

      if (proxy_socket == NULL)
        return FALSE;

      if (dbus_address)
        real_dbus_address = g_strdup (dbus_address);
      else
        real_dbus_address = g_strdup_printf ("unix:path=%s", dbus_system_socket);

      g_ptr_array_add (dbus_proxy_argv, g_strdup (real_dbus_address));
      g_ptr_array_add (dbus_proxy_argv, g_strdup (proxy_socket));


      add_args (argv_array,
                "--bind", proxy_socket, "/run/dbus/system_bus_socket",
                NULL);
      *envp_p = g_environ_setenv (*envp_p, "DBUS_SYSTEM_BUS_ADDRESS", "unix:path=/run/dbus/system_bus_socket", TRUE);

      return TRUE;
    }
  return FALSE;
}
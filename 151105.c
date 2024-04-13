flatpak_run_add_session_dbus_args (GPtrArray *argv_array,
                                   char    ***envp_p,
                                   GPtrArray *dbus_proxy_argv,
                                   gboolean   unrestricted)
{
  const char *dbus_address = g_getenv ("DBUS_SESSION_BUS_ADDRESS");
  char *dbus_session_socket = NULL;
  g_autofree char *sandbox_socket_path = g_strdup_printf ("/run/user/%d/bus", getuid ());
  g_autofree char *sandbox_dbus_address = g_strdup_printf ("unix:path=/run/user/%d/bus", getuid ());

  if (dbus_address == NULL)
    return FALSE;

  dbus_session_socket = extract_unix_path_from_dbus_address (dbus_address);
  if (dbus_session_socket != NULL && unrestricted)
    {

      add_args (argv_array,
                "--bind", dbus_session_socket, sandbox_socket_path,
                NULL);
      *envp_p = g_environ_setenv (*envp_p, "DBUS_SESSION_BUS_ADDRESS", sandbox_dbus_address, TRUE);

      return TRUE;
    }
  else if (dbus_proxy_argv && dbus_address != NULL)
    {
      g_autofree char *proxy_socket = create_proxy_socket (".session-bus-proxy-XXXXXX");

      if (proxy_socket == NULL)
        return FALSE;

      g_ptr_array_add (dbus_proxy_argv, g_strdup (dbus_address));
      g_ptr_array_add (dbus_proxy_argv, g_strdup (proxy_socket));

      add_args (argv_array,
                "--bind", proxy_socket, sandbox_socket_path,
                NULL);
      *envp_p = g_environ_setenv (*envp_p, "DBUS_SESSION_BUS_ADDRESS", sandbox_dbus_address, TRUE);

      return TRUE;
    }

  return FALSE;
}
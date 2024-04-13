create_proxy_socket (char *template)
{
  g_autofree char *proxy_socket = g_build_filename (g_get_user_runtime_dir (), template, NULL);
  int fd;

  fd = g_mkstemp (proxy_socket);
  if (fd == -1)
    return NULL;

  close (fd);

  return g_steal_pointer (&proxy_socket);
}
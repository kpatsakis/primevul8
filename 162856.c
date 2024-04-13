collect_addresses (GSocket * socket, gchar ** ip, guint16 * port,
    gboolean remote, GError ** error)
{
  GSocketAddress *addr;

  if (remote)
    addr = g_socket_get_remote_address (socket, error);
  else
    addr = g_socket_get_local_address (socket, error);
  if (!addr)
    return FALSE;

  if (ip)
    *ip = g_inet_address_to_string (g_inet_socket_address_get_address
        (G_INET_SOCKET_ADDRESS (addr)));
  if (port)
    *port = g_inet_socket_address_get_port (G_INET_SOCKET_ADDRESS (addr));

  g_object_unref (addr);

  return TRUE;
}
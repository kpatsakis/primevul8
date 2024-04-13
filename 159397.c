polkit_system_bus_name_exists_sync (PolkitSubject   *subject,
                                    GCancellable    *cancellable,
                                    GError         **error)
{
  PolkitSystemBusName *name = POLKIT_SYSTEM_BUS_NAME (subject);
  GDBusConnection *connection;
  GVariant *result;
  gboolean ret;

  ret = FALSE;

  connection = g_bus_get_sync (G_BUS_TYPE_SYSTEM, cancellable, error);
  if (connection == NULL)
    goto out;

  result = g_dbus_connection_call_sync (connection,
                                        "org.freedesktop.DBus",   /* name */
                                        "/org/freedesktop/DBus",  /* object path */
                                        "org.freedesktop.DBus",   /* interface name */
                                        "NameHasOwner",           /* method */
                                        g_variant_new ("(s)", name->name),
                                        G_VARIANT_TYPE ("(b)"),
                                        G_DBUS_CALL_FLAGS_NONE,
                                        -1,
                                        cancellable,
                                        error);
  if (result == NULL)
    goto out;

  g_variant_get (result, "(b)", &ret);
  g_variant_unref (result);

 out:
  if (connection != NULL)
    g_object_unref (connection);
  return ret;
}
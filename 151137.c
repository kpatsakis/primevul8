add_document_portal_args (GPtrArray  *argv_array,
                          const char *app_id)
{
  g_autoptr(GDBusConnection) session_bus = NULL;
  g_autofree char *doc_mount_path = NULL;

  session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
  if (session_bus)
    {
      g_autoptr(GError) local_error = NULL;
      g_autoptr(GDBusMessage) reply = NULL;
      g_autoptr(GDBusMessage) msg =
        g_dbus_message_new_method_call ("org.freedesktop.portal.Documents",
                                        "/org/freedesktop/portal/documents",
                                        "org.freedesktop.portal.Documents",
                                        "GetMountPoint");
      g_dbus_message_set_body (msg, g_variant_new ("()"));
      reply =
        g_dbus_connection_send_message_with_reply_sync (session_bus, msg,
                                                        G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                        30000,
                                                        NULL,
                                                        NULL,
                                                        NULL);
      if (reply)
        {
          if (g_dbus_message_to_gerror (reply, &local_error))
            {
              g_message ("Can't get document portal: %s\n", local_error->message);
            }
          else
            {
              g_autofree char *src_path = NULL;
              g_autofree char *dst_path = NULL;
              g_variant_get (g_dbus_message_get_body (reply),
                             "(^ay)", &doc_mount_path);

              src_path = g_strdup_printf ("%s/by-app/%s",
                                          doc_mount_path, app_id);
              dst_path = g_strdup_printf ("/run/user/%d/doc", getuid ());
              add_args (argv_array, "--bind", src_path, dst_path, NULL);
            }
        }
    }
}
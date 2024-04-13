register_display (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        GError *error = NULL;

        priv = gdm_display_get_instance_private (self);

        error = NULL;
        priv->connection = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, &error);
        if (priv->connection == NULL) {
                g_critical ("error getting system bus: %s", error->message);
                g_error_free (error);
                exit (EXIT_FAILURE);
        }

        priv->object_skeleton = g_dbus_object_skeleton_new (priv->id);
        priv->display_skeleton = GDM_DBUS_DISPLAY (gdm_dbus_display_skeleton_new ());

        g_signal_connect_object (priv->display_skeleton, "handle-get-id",
                                 G_CALLBACK (handle_get_id), self, 0);
        g_signal_connect_object (priv->display_skeleton, "handle-get-remote-hostname",
                                 G_CALLBACK (handle_get_remote_hostname), self, 0);
        g_signal_connect_object (priv->display_skeleton, "handle-get-seat-id",
                                 G_CALLBACK (handle_get_seat_id), self, 0);
        g_signal_connect_object (priv->display_skeleton, "handle-get-x11-display-name",
                                 G_CALLBACK (handle_get_x11_display_name), self, 0);
        g_signal_connect_object (priv->display_skeleton, "handle-is-local",
                                 G_CALLBACK (handle_is_local), self, 0);
        g_signal_connect_object (priv->display_skeleton, "handle-is-initial",
                                 G_CALLBACK (handle_is_initial), self, 0);

        g_dbus_object_skeleton_add_interface (priv->object_skeleton,
                                              G_DBUS_INTERFACE_SKELETON (priv->display_skeleton));

        return TRUE;
}
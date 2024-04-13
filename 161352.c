gdm_display_start_greeter_session (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;
        GdmSession    *session;
        char          *display_name;
        char          *seat_id;
        char          *hostname;
        char          *auth_file = NULL;

        priv = gdm_display_get_instance_private (self);
        g_return_if_fail (g_strcmp0 (priv->session_class, "greeter") == 0);

        g_debug ("GdmDisplay: Running greeter");

        display_name = NULL;
        seat_id = NULL;
        hostname = NULL;

        g_object_get (self,
                      "x11-display-name", &display_name,
                      "seat-id", &seat_id,
                      "remote-hostname", &hostname,
                      NULL);
        if (priv->access_file != NULL) {
                auth_file = gdm_display_access_file_get_path (priv->access_file);
        }

        g_debug ("GdmDisplay: Creating greeter for %s %s", display_name, hostname);

        g_signal_connect_object (priv->launch_environment,
                                 "opened",
                                 G_CALLBACK (on_launch_environment_session_opened),
                                 self, 0);
        g_signal_connect_object (priv->launch_environment,
                                 "started",
                                 G_CALLBACK (on_launch_environment_session_started),
                                 self, 0);
        g_signal_connect_object (priv->launch_environment,
                                 "stopped",
                                 G_CALLBACK (on_launch_environment_session_stopped),
                                 self, 0);
        g_signal_connect_object (priv->launch_environment,
                                 "exited",
                                 G_CALLBACK (on_launch_environment_session_exited),
                                 self, 0);
        g_signal_connect_object (priv->launch_environment,
                                 "died",
                                 G_CALLBACK (on_launch_environment_session_died),
                                 self, 0);

        if (auth_file != NULL) {
                g_object_set (priv->launch_environment,
                              "x11-authority-file", auth_file,
                              NULL);
        }

        gdm_launch_environment_start (priv->launch_environment);

        session = gdm_launch_environment_get_session (priv->launch_environment);
        g_object_set (G_OBJECT (session),
                      "display-is-initial", priv->is_initial,
                      NULL);

        g_free (display_name);
        g_free (seat_id);
        g_free (hostname);
        g_free (auth_file);
}
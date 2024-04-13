gdm_display_stop_greeter_session (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);

        if (priv->launch_environment != NULL) {

                g_signal_handlers_disconnect_by_func (priv->launch_environment,
                                                      G_CALLBACK (on_launch_environment_session_opened),
                                                      self);
                g_signal_handlers_disconnect_by_func (priv->launch_environment,
                                                      G_CALLBACK (on_launch_environment_session_started),
                                                      self);
                g_signal_handlers_disconnect_by_func (priv->launch_environment,
                                                      G_CALLBACK (on_launch_environment_session_stopped),
                                                      self);
                g_signal_handlers_disconnect_by_func (priv->launch_environment,
                                                      G_CALLBACK (on_launch_environment_session_exited),
                                                      self);
                g_signal_handlers_disconnect_by_func (priv->launch_environment,
                                                      G_CALLBACK (on_launch_environment_session_died),
                                                      self);
                gdm_launch_environment_stop (priv->launch_environment);
                g_clear_object (&priv->launch_environment);
        }
}
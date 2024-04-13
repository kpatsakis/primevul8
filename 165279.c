gs_window_set_user_switch_enabled (GSWindow *window,
                                   gboolean  user_switch_enabled)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        window->priv->user_switch_enabled = user_switch_enabled;
}
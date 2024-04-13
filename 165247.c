gs_window_set_logout_enabled (GSWindow *window,
                              gboolean  logout_enabled)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        window->priv->logout_enabled = logout_enabled;
}
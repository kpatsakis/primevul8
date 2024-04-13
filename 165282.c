gs_window_set_keyboard_enabled (GSWindow *window,
                                gboolean  enabled)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        window->priv->keyboard_enabled = enabled;
}
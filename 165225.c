gs_window_get_monitor (GSWindow *window)
{
        g_return_val_if_fail (GS_IS_WINDOW (window), -1);

        return window->priv->monitor;
}
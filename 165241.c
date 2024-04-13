gs_window_is_obscured (GSWindow *window)
{
        g_return_val_if_fail (GS_IS_WINDOW (window), FALSE);

        return window->priv->obscured;
}
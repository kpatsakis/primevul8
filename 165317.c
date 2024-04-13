gs_window_is_dialog_up (GSWindow *window)
{
        g_return_val_if_fail (GS_IS_WINDOW (window), FALSE);

        return window->priv->dialog_up;
}
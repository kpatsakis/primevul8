add_popup_dialog_idle (GSWindow *window)
{
        window->priv->popup_dialog_idle_id = g_idle_add ((GSourceFunc)popup_dialog_idle, window);
}
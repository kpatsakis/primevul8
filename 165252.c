remove_popup_dialog_idle (GSWindow *window)
{
        if (window->priv->popup_dialog_idle_id != 0) {
                g_source_remove (window->priv->popup_dialog_idle_id);
                window->priv->popup_dialog_idle_id = 0;
        }
}
popup_dialog_idle (GSWindow *window)
{
        popup_dialog (window);

        window->priv->popup_dialog_idle_id = 0;

        return FALSE;
}
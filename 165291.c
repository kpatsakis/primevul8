window_set_dialog_up (GSWindow *window,
                      gboolean  dialog_up)
{
        if (window->priv->dialog_up == dialog_up) {
                return;
        }

        window->priv->dialog_up = dialog_up;
        g_object_notify (G_OBJECT (window), "dialog-up");
}
popdown_dialog (GSWindow *window)
{
        gs_window_dialog_finish (window);

        set_invisible_cursor (gtk_widget_get_window (GTK_WIDGET (window)), TRUE);

        window_set_dialog_up (window, FALSE);

        /* reset the pointer positions */
        window->priv->last_x = -1;
        window->priv->last_y = -1;

        if (window->priv->lock_box != NULL) {
                gtk_container_remove (GTK_CONTAINER (window->priv->vbox), GTK_WIDGET (window->priv->lock_box));
                window->priv->lock_box = NULL;
        }

        remove_popup_dialog_idle (window);
        remove_command_watches (window);
}
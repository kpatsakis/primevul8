info_bar_timeout (GSWindow *window)
{
        window->priv->info_bar_timer_id = 0;
        gtk_widget_hide (window->priv->info_bar);
        return FALSE;
}
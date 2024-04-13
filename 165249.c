lock_plug_removed (GtkWidget *widget,
                   GSWindow  *window)
{
        gtk_widget_hide (widget);
        gtk_container_remove (GTK_CONTAINER (window->priv->vbox), GTK_WIDGET (window->priv->lock_box));
        window->priv->lock_box = NULL;

        return TRUE;
}
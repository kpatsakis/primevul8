create_info_bar (GSWindow *window)
{
        window->priv->info_bar = gtk_info_bar_new ();
        gtk_widget_set_no_show_all (window->priv->info_bar, TRUE);
        gtk_box_pack_end (GTK_BOX (window->priv->vbox), window->priv->info_bar, FALSE, FALSE, 0);
}
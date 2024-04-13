create_lock_socket (GSWindow *window,
                    guint32   id)
{
        window->priv->lock_socket = gtk_socket_new ();
        window->priv->lock_box = gtk_alignment_new (0.5, 0.5, 0, 0);
        gtk_widget_show (window->priv->lock_box);
        gtk_box_pack_start (GTK_BOX (window->priv->vbox), window->priv->lock_box, TRUE, TRUE, 0);

        gtk_container_add (GTK_CONTAINER (window->priv->lock_box), window->priv->lock_socket);

        g_signal_connect (window->priv->lock_socket, "show",
                          G_CALLBACK (lock_socket_show), window);
        g_signal_connect (window->priv->lock_socket, "destroy",
                          G_CALLBACK (lock_socket_destroyed), window);
        g_signal_connect (window->priv->lock_socket, "plug_added",
                          G_CALLBACK (lock_plug_added), window);
        g_signal_connect (window->priv->lock_socket, "plug_removed",
                          G_CALLBACK (lock_plug_removed), window);

        gtk_socket_add_id (GTK_SOCKET (window->priv->lock_socket), id);

        if (window->priv->keyboard_enabled) {
                embed_keyboard (window);
        }
}
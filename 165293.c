create_keyboard_socket (GSWindow *window,
                        guint32   id)
{
        int height;

        height = (gdk_screen_get_height (gtk_widget_get_screen (GTK_WIDGET (window)))) / 4;

        window->priv->keyboard_socket = gtk_socket_new ();
        gtk_widget_set_size_request (window->priv->keyboard_socket, -1, height);

        g_signal_connect (window->priv->keyboard_socket, "destroy",
                          G_CALLBACK (keyboard_socket_destroyed), window);
        g_signal_connect (window->priv->keyboard_socket, "plug_added",
                          G_CALLBACK (keyboard_plug_added), window);
        g_signal_connect (window->priv->keyboard_socket, "plug_removed",
                          G_CALLBACK (keyboard_plug_removed), window);
        gtk_box_pack_start (GTK_BOX (window->priv->vbox), window->priv->keyboard_socket, FALSE, FALSE, 0);
        gtk_socket_add_id (GTK_SOCKET (window->priv->keyboard_socket), id);
}
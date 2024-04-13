gs_window_move_resize_window (GSWindow *window,
                              gboolean  move,
                              gboolean  resize)
{
        GtkWidget *widget;

        widget = GTK_WIDGET (window);

        g_assert (gtk_widget_get_realized (widget));

        gs_debug ("Move and/or resize window on monitor %d: x=%d y=%d w=%d h=%d",
                  window->priv->monitor,
                  window->priv->geometry.x,
                  window->priv->geometry.y,
                  window->priv->geometry.width,
                  window->priv->geometry.height);

        if (move && resize) {
                gdk_window_move_resize (gtk_widget_get_window (widget),
                                        window->priv->geometry.x,
                                        window->priv->geometry.y,
                                        window->priv->geometry.width,
                                        window->priv->geometry.height);
        } else if (move) {
                gdk_window_move (gtk_widget_get_window (widget),
                                 window->priv->geometry.x,
                                 window->priv->geometry.y);
        } else if (resize) {
                gdk_window_resize (gtk_widget_get_window (widget),
                                   window->priv->geometry.width,
                                   window->priv->geometry.height);
        }
}
gs_window_real_size_request (GtkWidget      *widget,
                             GtkRequisition *requisition)
{
        GSWindow      *window;
        GtkBin        *bin;
        GdkRectangle   old_geometry;
        int            position_changed = FALSE;
        int            size_changed = FALSE;

        window = GS_WINDOW (widget);
        bin = GTK_BIN (widget);

        if (gtk_bin_get_child (bin) && gtk_widget_get_visible (gtk_bin_get_child (bin))) {
                gtk_widget_size_request (gtk_bin_get_child (bin), requisition);
        }

        old_geometry = window->priv->geometry;

        update_geometry (window);

        requisition->width  = window->priv->geometry.width;
        requisition->height = window->priv->geometry.height;

        if (!gtk_widget_get_realized (widget)) {
                return;
        }

        if (old_geometry.width  != window->priv->geometry.width ||
            old_geometry.height != window->priv->geometry.height) {
                size_changed = TRUE;
        }

        if (old_geometry.x != window->priv->geometry.x ||
            old_geometry.y != window->priv->geometry.y) {
                position_changed = TRUE;
        }

        gs_window_move_resize_window (window, position_changed, size_changed);
}
gs_window_clear_to_background_surface (GSWindow *window)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        if (!gtk_widget_get_visible (GTK_WIDGET (window))) {
                return;
        }

        if (window->priv->background_surface == NULL) {
                return;
        }

        gs_debug ("Clearing window to background pixmap");
        gs_window_reset_background_surface (window);
}
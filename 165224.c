gs_window_override_user_time (GSWindow *window)
{
        guint32 ev_time = gtk_get_current_event_time ();

        if (ev_time == 0) {
                gint ev_mask = gtk_widget_get_events (GTK_WIDGET (window));
                if (!(ev_mask & GDK_PROPERTY_CHANGE_MASK)) {
                        gtk_widget_add_events (GTK_WIDGET (window),
                                               GDK_PROPERTY_CHANGE_MASK);
                }

                /*
                 * NOTE: Last resort for D-BUS or other non-interactive
                 *       openings.  Causes roundtrip to server.  Lame.
                 */
                ev_time = gdk_x11_get_server_time (gtk_widget_get_window (GTK_WIDGET (window)));
        }

        gdk_x11_window_set_user_time (gtk_widget_get_window (GTK_WIDGET (window)), ev_time);
}
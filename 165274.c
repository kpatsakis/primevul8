gs_window_real_key_press_event (GtkWidget   *widget,
                                GdkEventKey *event)
{
        /*g_message ("KEY PRESS state: %u keyval %u", event->state, event->keyval);*/

        /* Ignore brightness keys */
        if (event->hardware_keycode == 101 || event->hardware_keycode == 212) {
                gs_debug ("Ignoring brightness keys");
                return TRUE;
        }

        maybe_handle_activity (GS_WINDOW (widget));

        queue_key_event (GS_WINDOW (widget), event);

        if (GTK_WIDGET_CLASS (gs_window_parent_class)->key_press_event) {
                GTK_WIDGET_CLASS (gs_window_parent_class)->key_press_event (widget, event);
        }

        return TRUE;
}
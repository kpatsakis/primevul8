gs_window_real_grab_broken (GtkWidget          *widget,
                            GdkEventGrabBroken *event)
{
        if (event->grab_window != NULL) {
                gs_debug ("Grab broken on window %X %s, new grab on window %X",
                          (guint32) GDK_WINDOW_XID (event->window),
                          event->keyboard ? "keyboard" : "pointer",
                          (guint32) GDK_WINDOW_XID (event->grab_window));
        } else {
                gs_debug ("Grab broken on window %X %s, new grab is outside application",
                          (guint32) GDK_WINDOW_XID (event->window),
                          event->keyboard ? "keyboard" : "pointer");
        }

        return FALSE;
}
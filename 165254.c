select_popup_events (void)
{
        XWindowAttributes attr;
        unsigned long     events;

        gdk_error_trap_push ();

        memset (&attr, 0, sizeof (attr));
        XGetWindowAttributes (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), GDK_ROOT_WINDOW (), &attr);

        events = SubstructureNotifyMask | attr.your_event_mask;
        XSelectInput (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), GDK_ROOT_WINDOW (), events);

        gdk_error_trap_pop_ignored ();
}
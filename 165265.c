window_select_shape_events (GSWindow *window)
{
#ifdef HAVE_SHAPE_EXT
        unsigned long events;
        int           shape_error_base;

        gdk_error_trap_push ();

        if (XShapeQueryExtension (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), &window->priv->shape_event_base, &shape_error_base)) {
                events = ShapeNotifyMask;
                XShapeSelectInput (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), GDK_WINDOW_XID (gtk_widget_get_window (GTK_WIDGET (window))), events);
        }

        gdk_error_trap_pop_ignored ();
#endif
}
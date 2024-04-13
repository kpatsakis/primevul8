gs_window_real_visibility_notify_event (GtkWidget          *widget,
                                        GdkEventVisibility *event)
{
        switch (event->state) {
        case GDK_VISIBILITY_FULLY_OBSCURED:
                window_set_obscured (GS_WINDOW (widget), TRUE);
                break;
        case GDK_VISIBILITY_PARTIAL:
                break;
        case GDK_VISIBILITY_UNOBSCURED:
                window_set_obscured (GS_WINDOW (widget), FALSE);
                break;
        default:
                break;
        }

        return FALSE;
}
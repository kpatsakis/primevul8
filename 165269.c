gs_window_real_get_preferred_height (GtkWidget *widget,
                                gint      *minimal_height,
                                gint      *natural_height)
{
        GtkRequisition requisition;

        gs_window_real_size_request (widget, &requisition);

        *minimal_height = *natural_height = requisition.height;
}
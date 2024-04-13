gs_window_real_get_preferred_width (GtkWidget *widget,
                               gint      *minimal_width,
                               gint      *natural_width)
{
        GtkRequisition requisition;

        gs_window_real_size_request (widget, &requisition);

        *minimal_width = *natural_width = requisition.width;
}
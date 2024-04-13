shade_background (GtkWidget    *widget,
               cairo_t      *cr,
               GSWindow     *window)
{
        cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.7);
        cairo_paint (cr);
        return FALSE;
}
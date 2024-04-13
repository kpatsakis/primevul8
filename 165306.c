add_emit_deactivated_idle (GSWindow *window)
{
        g_idle_add ((GSourceFunc)emit_deactivated_idle, window);
}
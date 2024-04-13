emit_deactivated_idle (GSWindow *window)
{
        g_signal_emit (window, signals [DEACTIVATED], 0);

        return FALSE;
}
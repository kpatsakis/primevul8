gs_window_cancel_unlock_request (GSWindow  *window)
{
        /* FIXME: This is a bit of a hammer approach...
         * Maybe we should send a delete-event to
         * the plug?
         */
        g_return_if_fail (GS_IS_WINDOW (window));

        popdown_dialog (window);
}
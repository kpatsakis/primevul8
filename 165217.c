gs_window_set_logout_timeout (GSWindow *window,
                              glong     logout_timeout)
{
        g_return_if_fail (GS_IS_WINDOW (window));

        if (logout_timeout < 0) {
                window->priv->logout_timeout = 0;
        } else {
                window->priv->logout_timeout = logout_timeout;
        }
}
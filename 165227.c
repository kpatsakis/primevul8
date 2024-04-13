window_set_obscured (GSWindow *window,
                     gboolean  obscured)
{
        if (window->priv->obscured == obscured) {
                return;
        }

        window->priv->obscured = obscured;
        g_object_notify (G_OBJECT (window), "obscured");
}
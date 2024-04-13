queue_finish (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        if (priv->finish_idle_id == 0) {
                priv->finish_idle_id = g_idle_add ((GSourceFunc)finish_idle, self);
        }
}
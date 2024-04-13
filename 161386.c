finish_idle (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        priv->finish_idle_id = 0;
        /* finish may end up finalizing object */
        gdm_display_finish (self);
        return FALSE;
}
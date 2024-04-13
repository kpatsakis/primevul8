gdm_display_init (GdmDisplay *self)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);

        priv->creation_time = time (NULL);
}
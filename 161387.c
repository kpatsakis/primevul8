_gdm_display_set_x11_display_number (GdmDisplay     *self,
                                     int             num)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        priv->x11_display_number = num;
}
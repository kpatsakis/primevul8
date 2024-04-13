gdm_display_get_x11_display_number (GdmDisplay *self,
                                    int        *number,
                                    GError    **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (number != NULL) {
                *number = priv->x11_display_number;
        }

        return TRUE;
}
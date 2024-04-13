gdm_display_get_seat_id (GdmDisplay *self,
                         char      **seat_id,
                         GError    **error)
{
        GdmDisplayPrivate *priv;

        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        priv = gdm_display_get_instance_private (self);
        if (seat_id != NULL) {
                *seat_id = g_strdup (priv->seat_id);
        }

        return TRUE;
}
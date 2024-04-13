gdm_display_real_prepare (GdmDisplay *self)
{
        g_return_val_if_fail (GDM_IS_DISPLAY (self), FALSE);

        g_debug ("GdmDisplay: prepare display");

        _gdm_display_set_status (self, GDM_DISPLAY_PREPARED);

        return TRUE;
}
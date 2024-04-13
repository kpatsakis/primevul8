self_destruct (GdmDisplay *self)
{
        g_object_ref (self);
        if (gdm_display_get_status (self) == GDM_DISPLAY_MANAGED) {
                gdm_display_unmanage (self);
        }

        if (gdm_display_get_status (self) != GDM_DISPLAY_FINISHED) {
                queue_finish (self);
        }
        g_object_unref (self);
}
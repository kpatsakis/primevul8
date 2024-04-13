on_launch_environment_session_opened (GdmLaunchEnvironment *launch_environment,
                                      GdmDisplay           *self)
{
        char       *session_id;

        g_debug ("GdmDisplay: Greeter session opened");
        session_id = gdm_launch_environment_get_session_id (launch_environment);
        _gdm_display_set_session_id (self, session_id);
        g_free (session_id);
}
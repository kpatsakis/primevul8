on_launch_environment_session_exited (GdmLaunchEnvironment *launch_environment,
                                      int                   code,
                                      GdmDisplay           *self)
{
        g_debug ("GdmDisplay: Greeter exited: %d", code);
        self_destruct (self);
}
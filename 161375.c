on_launch_environment_session_stopped (GdmLaunchEnvironment *launch_environment,
                                       GdmDisplay           *self)
{
        g_debug ("GdmDisplay: Greeter stopped");
        self_destruct (self);
}
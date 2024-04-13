void qemu_input_event_sync(void)
{
    if (!runstate_is_running() && !runstate_check(RUN_STATE_SUSPENDED)) {
        return;
    }

    replay_input_sync_event();
}
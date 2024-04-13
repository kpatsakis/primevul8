void qemu_input_event_send(QemuConsole *src, InputEvent *evt)
{
    if (!runstate_is_running() && !runstate_check(RUN_STATE_SUSPENDED)) {
        return;
    }

    replay_input_event(src, evt);
}
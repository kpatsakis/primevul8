void qemu_input_queue_rel(QemuConsole *src, InputAxis axis, int value)
{
    InputEvent *evt;
    evt = qemu_input_event_new_move(INPUT_EVENT_KIND_REL, axis, value);
    qemu_input_event_send(src, evt);
    qapi_free_InputEvent(evt);
}
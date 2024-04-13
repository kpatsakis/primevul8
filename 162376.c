void qemu_input_queue_btn(QemuConsole *src, InputButton btn, bool down)
{
    InputEvent *evt;
    evt = qemu_input_event_new_btn(btn, down);
    qemu_input_event_send(src, evt);
    qapi_free_InputEvent(evt);
}
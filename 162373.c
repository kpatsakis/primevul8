void qemu_input_event_send_impl(QemuConsole *src, InputEvent *evt)
{
    QemuInputHandlerState *s;

    qemu_input_event_trace(src, evt);

    /* pre processing */
    if (graphic_rotate && (evt->type == INPUT_EVENT_KIND_ABS)) {
            qemu_input_transform_abs_rotate(evt);
    }

    /* send event */
    s = qemu_input_find_handler(1 << evt->type, src);
    if (!s) {
        return;
    }
    s->handler->event(s->dev, src, evt);
    s->events++;
}
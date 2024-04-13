bool qemu_input_is_absolute(void)
{
    QemuInputHandlerState *s;

    s = qemu_input_find_handler(INPUT_EVENT_MASK_REL | INPUT_EVENT_MASK_ABS,
                                NULL);
    return (s != NULL) && (s->handler->mask & INPUT_EVENT_MASK_ABS);
}
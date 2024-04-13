void qemu_input_update_buttons(QemuConsole *src, uint32_t *button_map,
                               uint32_t button_old, uint32_t button_new)
{
    InputButton btn;
    uint32_t mask;

    for (btn = 0; btn < INPUT_BUTTON__MAX; btn++) {
        mask = button_map[btn];
        if ((button_old & mask) == (button_new & mask)) {
            continue;
        }
        qemu_input_queue_btn(src, btn, button_new & mask);
    }
}
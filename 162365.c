static void qemu_input_event_trace(QemuConsole *src, InputEvent *evt)
{
    const char *name;
    int qcode, idx = -1;
    InputKeyEvent *key;
    InputBtnEvent *btn;
    InputMoveEvent *move;

    if (src) {
        idx = qemu_console_get_index(src);
    }
    switch (evt->type) {
    case INPUT_EVENT_KIND_KEY:
        key = evt->u.key.data;
        switch (key->key->type) {
        case KEY_VALUE_KIND_NUMBER:
            qcode = qemu_input_key_number_to_qcode(key->key->u.number.data);
            name = QKeyCode_lookup[qcode];
            trace_input_event_key_number(idx, key->key->u.number.data,
                                         name, key->down);
            break;
        case KEY_VALUE_KIND_QCODE:
            name = QKeyCode_lookup[key->key->u.qcode.data];
            trace_input_event_key_qcode(idx, name, key->down);
            break;
        case KEY_VALUE_KIND__MAX:
            /* keep gcc happy */
            break;
        }
        break;
    case INPUT_EVENT_KIND_BTN:
        btn = evt->u.btn.data;
        name = InputButton_lookup[btn->button];
        trace_input_event_btn(idx, name, btn->down);
        break;
    case INPUT_EVENT_KIND_REL:
        move = evt->u.rel.data;
        name = InputAxis_lookup[move->axis];
        trace_input_event_rel(idx, name, move->value);
        break;
    case INPUT_EVENT_KIND_ABS:
        move = evt->u.abs.data;
        name = InputAxis_lookup[move->axis];
        trace_input_event_abs(idx, name, move->value);
        break;
    case INPUT_EVENT_KIND__MAX:
        /* keep gcc happy */
        break;
    }
}
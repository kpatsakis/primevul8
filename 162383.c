InputEvent *qemu_input_event_new_btn(InputButton btn, bool down)
{
    InputEvent *evt = g_new0(InputEvent, 1);
    evt->u.btn.data = g_new0(InputBtnEvent, 1);
    evt->type = INPUT_EVENT_KIND_BTN;
    evt->u.btn.data->button = btn;
    evt->u.btn.data->down = down;
    return evt;
}
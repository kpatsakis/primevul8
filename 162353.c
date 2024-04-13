MouseInfoList *qmp_query_mice(Error **errp)
{
    MouseInfoList *mice_list = NULL;
    MouseInfoList *info;
    QemuInputHandlerState *s;
    bool current = true;

    QTAILQ_FOREACH(s, &handlers, node) {
        if (!(s->handler->mask &
              (INPUT_EVENT_MASK_REL | INPUT_EVENT_MASK_ABS))) {
            continue;
        }

        info = g_new0(MouseInfoList, 1);
        info->value = g_new0(MouseInfo, 1);
        info->value->index = s->id;
        info->value->name = g_strdup(s->handler->name);
        info->value->absolute = s->handler->mask & INPUT_EVENT_MASK_ABS;
        info->value->current = current;

        current = false;
        info->next = mice_list;
        mice_list = info;
    }

    return mice_list;
}
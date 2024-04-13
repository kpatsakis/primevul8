unres_schema_find(struct unres_schema *unres, int start_on_backwards, void *item, enum UNRES_ITEM type)
{
    int i;
    struct unres_list_uniq *aux_uniq1, *aux_uniq2;

    if (!unres->count) {
        return -1;
    }

    if (start_on_backwards >= 0) {
        i = start_on_backwards;
    } else {
        i = unres->count - 1;
    }
    for (; i > -1; i--) {
        if (unres->type[i] != type) {
            continue;
        }
        if (type != UNRES_LIST_UNIQ) {
            if (unres->item[i] == item) {
                break;
            }
        } else {
            aux_uniq1 = (struct unres_list_uniq *)unres->item[i];
            aux_uniq2 = (struct unres_list_uniq *)item;
            if ((aux_uniq1->list == aux_uniq2->list) && ly_strequal(aux_uniq1->expr, aux_uniq2->expr, 0)) {
                break;
            }
        }
    }

    return i;
}
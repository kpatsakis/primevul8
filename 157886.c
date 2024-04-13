growable_int_array_add(growable_int_array *arr, int item) {
    if (arr->num_items >= arr->size) {
        arr->size *= 2;
        arr->items = realloc(arr->items, arr->size * sizeof(*arr->items));
        if (!arr->items) {
            return 0;
        }
    }

    arr->items[arr->num_items] = item;
    arr->num_items++;
    return 1;
}
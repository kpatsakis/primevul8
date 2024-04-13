growable_int_array_init(growable_int_array *arr, size_t initial_size) {
    assert(initial_size > 0);
    arr->items = malloc(initial_size * sizeof(*arr->items));
    arr->size = initial_size;
    arr->num_items = 0;

    return arr->items != NULL;
}
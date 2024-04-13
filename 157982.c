growable_int_array_deallocate(growable_int_array *arr) {
    free(arr->items);
}
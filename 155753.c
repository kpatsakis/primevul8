hashTableClear(HASH_TABLE *table) {
  size_t i;
  for (i = 0; i < table->size; i++) {
    table->mem->free_fcn(table->v[i]);
    table->v[i] = NULL;
  }
  table->used = 0;
}
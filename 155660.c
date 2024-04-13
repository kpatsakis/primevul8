hashTableIterInit(HASH_TABLE_ITER *iter, const HASH_TABLE *table) {
  iter->p = table->v;
  iter->end = iter->p ? iter->p + table->size : NULL;
}
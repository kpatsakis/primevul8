hashTableIterNext(HASH_TABLE_ITER *iter) {
  while (iter->p != iter->end) {
    NAMED *tem = *(iter->p)++;
    if (tem)
      return tem;
  }
  return NULL;
}
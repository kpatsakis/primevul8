hashTableInit(HASH_TABLE *p, const XML_Memory_Handling_Suite *ms) {
  p->power = 0;
  p->size = 0;
  p->used = 0;
  p->v = NULL;
  p->mem = ms;
}
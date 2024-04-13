poolDestroy(STRING_POOL *pool) {
  BLOCK *p = pool->blocks;
  while (p) {
    BLOCK *tem = p->next;
    pool->mem->free_fcn(p);
    p = tem;
  }
  p = pool->freeBlocks;
  while (p) {
    BLOCK *tem = p->next;
    pool->mem->free_fcn(p);
    p = tem;
  }
}
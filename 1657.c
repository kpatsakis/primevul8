mrb_ary_decref(mrb_state *mrb, mrb_shared_array *shared)
{
  shared->refcnt--;
  if (shared->refcnt == 0) {
    mrb_free(mrb, shared->ptr);
    mrb_free(mrb, shared);
  }
}
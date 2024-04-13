exec_end_call_list(void)
{
  OnigEndCallListItemType* prev;
  void (*func)(void);

  while (EndCallTop != 0) {
    func = EndCallTop->func;
    (*func)();

    prev = EndCallTop;
    EndCallTop = EndCallTop->next;
    xfree(prev);
  }
}
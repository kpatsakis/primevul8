static void ResetPixelList(PixelList *pixel_list)
{
  int
    level;

  register ListNode
    *root;

  register SkipList
    *list;

  register ssize_t
    channel;

  /*
    Reset the skip-list.
  */
  for (channel=0; channel < 5; channel++)
  {
    list=pixel_list->lists+channel;
    root=list->nodes+65536UL;
    list->level=0;
    for (level=0; level < 9; level++)
      root->next[level]=65536UL;
  }
  pixel_list->seed=pixel_list->signature++;
}
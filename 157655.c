merge_with_prev (
  FriBidiRun *second
)
{
  FriBidiRun *first;

  fribidi_assert (second);
  fribidi_assert (second->next);
  first = second->prev;
  fribidi_assert (first);

  first->next = second->next;
  first->next->prev = first;
  RL_LEN (first) += RL_LEN (second);
  if (second->next_isolate)
    second->next_isolate->prev_isolate = first;
  first->next_isolate = second->next_isolate;

  fribidi_free (second);
  return first;
}
int my_progress_callback(void *unused_data, enum LibRaw_progress state,
                         int iter, int expected)
{
  if (iter == 0)
    printf("CB: state=%x, expected %d iterations\n", state, expected);
  return 0;
}
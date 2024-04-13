int free_embedded_options(char ** options_list, int options_count)
{
  int i;

  for (i= 0; i < options_count; i++)
  {
    if (options_list[i])
      free(options_list[i]);
  }
  free(options_list);

  return 1;
}
char *get_next_file()
{
  char *ret;
  if (!queue)
    return NULL;
  if (qptr >= qsize)
    return NULL;
  pthread_mutex_lock(&qm);
  ret = queue[qptr++];
  pthread_mutex_unlock(&qm);
  return ret;
}
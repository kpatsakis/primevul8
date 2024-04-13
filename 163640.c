addselectionjob(Pool *pool, char **pieces, int npieces, Queue *jobqueue, int type, int keyname)
{
  Id job;
  int i, r = 0;
  int selflags;
  Queue sel;
  char *sp;

  for (i = 0; job2str[i].str; i++)
    if (!strcmp(pieces[0], job2str[i].str))
      break;
  if (!job2str[i].str)
    return pool_error(pool, -1, "selstr2job: unknown job '%s'", pieces[0]);
  job = job2str[i].job;
  if (npieces > 3)
    {
      char *flags = pieces[npieces - 1];
      if (*flags == '[' && flags[strlen(flags) - 1] == ']')
	{
	  npieces--;
	  flags++;
	  flags[strlen(flags) - 1] = 0;
	  job |= str2jobflags(pool, flags);
	}
    }
  if (npieces < 4)
    return pool_error(pool, -1, "selstr2job: no selection flags");
  selflags = str2selflags(pool, pieces[npieces - 1]);
  /* re-join pieces */
  for (sp = pieces[2]; sp < pieces[npieces - 2]; sp++)
    if (*sp == 0)
      *sp = ' ';
  queue_init(&sel);
  if (selflags & (SELECTION_ADD | SELECTION_SUBTRACT | SELECTION_FILTER))
    {
      for (i = 0; i < jobqueue->count; i += 2)
	queue_push2(&sel, jobqueue->elements[i] & (SOLVER_SELECTMASK | SOLVER_SETMASK), jobqueue->elements[i + 1]);
      queue_empty(jobqueue);
    }
  if (!type)
    r = selection_make(pool, &sel, pieces[2], selflags);
  else if (type == SELECTIONJOB_MATCHDEPS)
    r = selection_make_matchdeps(pool, &sel, pieces[2], selflags, keyname, 0);
  else if (type == SELECTIONJOB_MATCHDEPID)
    r = selection_make_matchdepid(pool, &sel, testcase_str2dep(pool, pieces[2]), selflags, keyname, 0);
  else if (type == SELECTIONJOB_MATCHSOLVABLE)
    r = selection_make_matchsolvable(pool, &sel, testcase_str2solvid(pool, pieces[2]), selflags, keyname, 0);
  for (i = 0; i < sel.count; i += 2)
    queue_push2(jobqueue, job | sel.elements[i], sel.elements[i + 1]);
  queue_free(&sel);
  return r;
}
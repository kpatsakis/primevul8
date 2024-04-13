dump_genid(Pool *pool, Strqueue *sq, Id id, int cnt)
{
  struct oplist *op;
  char cntbuf[20];
  const char *s;

  if (ISRELDEP(id))
    {
      Reldep *rd = GETRELDEP(pool, id);
      for (op = oplist; op->flags; op++)
	if (rd->flags == op->flags)
	  break;
      cnt = dump_genid(pool, sq, rd->name, cnt);
      cnt = dump_genid(pool, sq, rd->evr, cnt);
      sprintf(cntbuf, "genid %2d: genid ", cnt++);
      s = pool_tmpjoin(pool, cntbuf, "op ", op->flags ? op->opname : "unknown");
    }
  else
    {
      sprintf(cntbuf, "genid %2d: genid ", cnt++);
      s = pool_tmpjoin(pool, cntbuf, id ? "lit " : "null", id ? pool_id2str(pool, id) : 0);
    }
  strqueue_push(sq, s);
  return cnt;
}
testcase_solverresult(Solver *solv, int resultflags)
{
  Pool *pool = solv->pool;
  int i, j;
  Id p, op;
  const char *s;
  char *result;
  Strqueue sq;

  strqueue_init(&sq);
  if ((resultflags & TESTCASE_RESULT_TRANSACTION) != 0)
    {
      Transaction *trans = solver_create_transaction(solv);
      Queue q;

      queue_init(&q);
      for (i = 0; class2str[i].str; i++)
	{
	  queue_empty(&q);
	  transaction_classify_pkgs(trans, SOLVER_TRANSACTION_KEEP_PSEUDO, class2str[i].class, 0, 0, &q);
	  for (j = 0; j < q.count; j++)
	    {
	      p = q.elements[j];
	      op = 0;
	      if (pool->installed && pool->solvables[p].repo == pool->installed)
		op = transaction_obs_pkg(trans, p);
	      s = pool_tmpjoin(pool, class2str[i].str, " ", testcase_solvid2str(pool, p));
	      if (op)
		s = pool_tmpjoin(pool, s, " ", testcase_solvid2str(pool, op));
	      strqueue_push(&sq, s);
	    }
	}
      queue_free(&q);
      transaction_free(trans);
    }
  if ((resultflags & TESTCASE_RESULT_PROBLEMS) != 0)
    {
      char *probprefix, *solprefix;
      int problem, solution, element;
      int pcnt, scnt;

      pcnt = solver_problem_count(solv);
      for (problem = 1; problem <= pcnt; problem++)
	{
	  Id rid, from, to, dep;
	  SolverRuleinfo rinfo;
	  rid = solver_findproblemrule(solv, problem);
	  s = testcase_problemid(solv, problem);
	  probprefix = solv_dupjoin("problem ", s, 0);
	  rinfo = solver_ruleinfo(solv, rid, &from, &to, &dep);
	  s = pool_tmpjoin(pool, probprefix, " info ", solver_problemruleinfo2str(solv, rinfo, from, to, dep));
	  strqueue_push(&sq, s);
	  scnt = solver_solution_count(solv, problem);
	  for (solution = 1; solution <= scnt; solution++)
	    {
	      s = testcase_solutionid(solv, problem, solution);
	      solprefix = solv_dupjoin(probprefix, " solution ", s);
	      element = 0;
	      while ((element = solver_next_solutionelement(solv, problem, solution, element, &p, &op)) != 0)
		{
		  if (p == SOLVER_SOLUTION_JOB)
		    s = pool_tmpjoin(pool, solprefix, " deljob ", testcase_job2str(pool, solv->job.elements[op - 1], solv->job.elements[op]));
		  else if (p > 0 && op == 0)
		    s = pool_tmpjoin(pool, solprefix, " erase ", testcase_solvid2str(pool, p));
		  else if (p > 0 && op > 0)
		    {
		      s = pool_tmpjoin(pool, solprefix, " replace ", testcase_solvid2str(pool, p));
		      s = pool_tmpappend(pool, s, " ", testcase_solvid2str(pool, op));
		    }
		  else if (p < 0 && op > 0)
		    s = pool_tmpjoin(pool, solprefix, " allow ", testcase_solvid2str(pool, op));
		  else
		    s = pool_tmpjoin(pool, solprefix, " unknown", 0);
		  strqueue_push(&sq, s);
		}
	      solv_free(solprefix);
	    }
	  solv_free(probprefix);
	}
    }

  if ((resultflags & TESTCASE_RESULT_ORPHANED) != 0)
    {
      Queue q;

      queue_init(&q);
      solver_get_orphaned(solv, &q);
      for (i = 0; i < q.count; i++)
	{
	  s = pool_tmpjoin(pool, "orphaned ", testcase_solvid2str(pool, q.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      queue_free(&q);
    }

  if ((resultflags & TESTCASE_RESULT_RECOMMENDED) != 0)
    {
      Queue qr, qs;

      queue_init(&qr);
      queue_init(&qs);
      solver_get_recommendations(solv, &qr, &qs, 0);
      for (i = 0; i < qr.count; i++)
	{
	  s = pool_tmpjoin(pool, "recommended ", testcase_solvid2str(pool, qr.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      for (i = 0; i < qs.count; i++)
	{
	  s = pool_tmpjoin(pool, "suggested ", testcase_solvid2str(pool, qs.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      queue_free(&qr);
      queue_free(&qs);
    }

  if ((resultflags & TESTCASE_RESULT_UNNEEDED) != 0)
    {
      Queue q, qf;

      queue_init(&q);
      queue_init(&qf);
      solver_get_unneeded(solv, &q, 0);
      solver_get_unneeded(solv, &qf, 1);
      for (i = j = 0; i < q.count; i++)
	{
	  /* we rely on qf containing a subset of q in the same order */
	  if (j < qf.count && q.elements[i] == qf.elements[j])
	    {
	      s = pool_tmpjoin(pool, "unneeded_filtered ", testcase_solvid2str(pool, q.elements[i]), 0);
	      j++;
	    }
	  else
	    s = pool_tmpjoin(pool, "unneeded ", testcase_solvid2str(pool, q.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      queue_free(&q);
      queue_free(&qf);
    }
  if ((resultflags & TESTCASE_RESULT_USERINSTALLED) != 0)
    {
      Queue q;
      solver_get_userinstalled(solv, &q, 0);
      for (i = 0; i < q.count; i++)
	{
	  s = pool_tmpjoin(pool, "userinstalled pkg ", testcase_solvid2str(pool, q.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      queue_empty(&q);
      solver_get_userinstalled(solv, &q, GET_USERINSTALLED_NAMES | GET_USERINSTALLED_INVERTED);
      for (i = 0; i < q.count; i++)
	{
	  s = pool_tmpjoin(pool, "autoinst name ", pool_id2str(pool, q.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      queue_free(&q);
    }
  if ((resultflags & TESTCASE_RESULT_ALTERNATIVES) != 0)
    {
      char *altprefix;
      Queue q, rq;
      int cnt;
      Id alternative;
      queue_init(&q);
      queue_init(&rq);
      cnt = solver_alternatives_count(solv);
      for (alternative = 1; alternative <= cnt; alternative++)
	{
	  Id id, from, chosen;
	  char num[20];
	  int type = solver_get_alternative(solv, alternative, &id, &from, &chosen, &q, 0);
	  altprefix = solv_dupjoin("alternative ", testcase_alternativeid(solv, type, id, from), " ");
	  strcpy(num, " 0 ");
	  if (type == SOLVER_ALTERNATIVE_TYPE_RECOMMENDS)
	    {
	      char *s = pool_tmpjoin(pool, altprefix, num, testcase_solvid2str(pool, from));
	      s = pool_tmpappend(pool, s, " recommends ", testcase_dep2str(pool, id));
	      strqueue_push(&sq, s);
	    }
	  else if (type == SOLVER_ALTERNATIVE_TYPE_RULE)
	    {
	      /* map choice rules back to pkg rules */
	      if (solver_ruleclass(solv, id) == SOLVER_RULE_CHOICE)
		id = solver_rule2pkgrule(solv, id);
	      if (solver_ruleclass(solv, id) == SOLVER_RULE_RECOMMENDS)
		id = solver_rule2pkgrule(solv, id);
	      solver_allruleinfos(solv, id, &rq);
	      for (i = 0; i < rq.count; i += 4)
		{
		  int rtype = rq.elements[i];
		  if ((rtype & SOLVER_RULE_TYPEMASK) == SOLVER_RULE_JOB)
		    {
		      const char *js = testcase_job2str(pool, rq.elements[i + 2], rq.elements[i + 3]);
		      char *s = pool_tmpjoin(pool, altprefix, num, "job ");
		      s = pool_tmpappend(pool, s, js, 0);
		      strqueue_push(&sq, s);
		    }
		  else if (rtype == SOLVER_RULE_PKG_REQUIRES)
		    {
		      char *s = pool_tmpjoin(pool, altprefix, num, testcase_solvid2str(pool, rq.elements[i + 1]));
		      s = pool_tmpappend(pool, s, " requires ", testcase_dep2str(pool, rq.elements[i + 3]));
		      strqueue_push(&sq, s);
		    }
		  else if (rtype == SOLVER_RULE_UPDATE || rtype == SOLVER_RULE_FEATURE)
		    {
		      const char *js = testcase_solvid2str(pool, rq.elements[i + 1]);
		      char *s = pool_tmpjoin(pool, altprefix, num, "update ");
		      s = pool_tmpappend(pool, s, js, 0);
		      strqueue_push(&sq, s);
		    }
		}
	    }
	  for (i = 0; i < q.count; i++)
	    {
	      Id p = q.elements[i];
	      if (i >= 9)
	        num[0] = '0' + (i + 1) / 10;
	      num[1] = '0' + (i + 1) % 10;
	      if (-p == chosen)
		s = pool_tmpjoin(pool, altprefix, num, "+ ");
	      else if (p < 0)
	        s = pool_tmpjoin(pool, altprefix, num, "- ");
	      else if (p >= 0)
	        s = pool_tmpjoin(pool, altprefix, num, "  ");
	      s = pool_tmpappend(pool, s,  testcase_solvid2str(pool, p < 0 ? -p : p), 0);
	      strqueue_push(&sq, s);
	    }
	  solv_free(altprefix);
	}
      queue_free(&q);
      queue_free(&rq);
    }
  if ((resultflags & TESTCASE_RESULT_RULES) != 0)
    {
      /* dump all rules */
      Id rid;
      SolverRuleinfo rclass;
      Queue q;
      int i;

      queue_init(&q);
      for (rid = 1; (rclass = solver_ruleclass(solv, rid)) != SOLVER_RULE_UNKNOWN; rid++)
	{
	  char *prefix = solv_dupjoin("rule ", testcase_rclass2str(rclass), " ");
	  prefix = solv_dupappend(prefix, testcase_ruleid(solv, rid), 0);
	  solver_ruleliterals(solv, rid, &q);
	  if (rclass == SOLVER_RULE_FEATURE && q.count == 1 && q.elements[0] == -SYSTEMSOLVABLE)
	    continue;
	  for (i = 0; i < q.count; i++)
	    {
	      Id p = q.elements[i];
	      const char *s;
	      if (p < 0)
		s = pool_tmpjoin(pool, prefix, " -", testcase_solvid2str(pool, -p));
	      else
		s = pool_tmpjoin(pool, prefix, "  ", testcase_solvid2str(pool, p));
	      strqueue_push(&sq, s);
	    }
	  solv_free(prefix);
	}
      queue_free(&q);
    }
  if ((resultflags & TESTCASE_RESULT_GENID) != 0)
    {
      for (i = 0 ; i < solv->job.count; i += 2)
	{
	  Id id, id2;
	  if (solv->job.elements[i] != (SOLVER_NOOP | SOLVER_SOLVABLE_PROVIDES))
	    continue;
	  id = solv->job.elements[i + 1];
	  s = testcase_dep2str(pool, id);
	  strqueue_push(&sq, pool_tmpjoin(pool, "genid dep ", s, 0));
	  if ((id2 = testcase_str2dep(pool, s)) != id)
	    {
	      s = pool_tmpjoin(pool, "genid roundtrip error: ", testcase_dep2str(pool, id2), 0);
	      strqueue_push(&sq, s);
	    }
	  dump_genid(pool, &sq, id, 1);
	}
    }
  if ((resultflags & TESTCASE_RESULT_REASON) != 0)
    {
      Queue whyq;
      queue_init(&whyq);
      FOR_POOL_SOLVABLES(p)
	{
	  Id info, p2, id;
          int reason = solver_describe_decision(solv, p, &info);
	  if (reason == SOLVER_REASON_UNRELATED)
	    continue;
	  if (reason == SOLVER_REASON_WEAKDEP)
	    {
	      solver_describe_weakdep_decision(solv, p, &whyq);
	      if (whyq.count)
		{
		  for (i = 0; i < whyq.count; i += 3)
		    {
		      reason = whyq.elements[i];
		      p2 = whyq.elements[i + 1];
		      id = whyq.elements[i + 2];
		      s = pool_tmpjoin(pool, "reason ", testcase_solvid2str(pool, p), 0);
		      s = pool_tmpappend(pool, s, " ", testcase_reason2str(reason));
		      s = pool_tmpappend(pool, s, " ", testcase_dep2str(pool, id));
		      if (p2)
		        s = pool_tmpappend(pool, s, " ", testcase_solvid2str(pool, p2));
		      strqueue_push(&sq, s);
		    }
		  continue;
		}
	    }
	  s = pool_tmpjoin(pool, "reason ", testcase_solvid2str(pool, p), 0);
	  s = pool_tmpappend(pool, s, " ", testcase_reason2str(reason));
	  if (info)
	    s = pool_tmpappend(pool, s, " ", testcase_ruleid(solv, info));
	  strqueue_push(&sq, s);
	}
      queue_free(&whyq);
    }
  if ((resultflags & TESTCASE_RESULT_CLEANDEPS) != 0)
    {
      Queue q;

      queue_init(&q);
      solver_get_cleandeps(solv, &q);
      for (i = 0; i < q.count; i++)
	{
	  s = pool_tmpjoin(pool, "cleandeps ", testcase_solvid2str(pool, q.elements[i]), 0);
	  strqueue_push(&sq, s);
	}
      queue_free(&q);
    }
  if ((resultflags & TESTCASE_RESULT_JOBS) != 0)
    {
      for (i = 0; i < solv->job.count; i += 2)
	{
	  s = (char *)testcase_job2str(pool, solv->job.elements[i], solv->job.elements[i + 1]);
	  s = pool_tmpjoin(pool, "job ", s, 0);
	  strqueue_push(&sq, s);
	}
    }
  strqueue_sort(&sq);
  result = strqueue_join(&sq);
  strqueue_free(&sq);
  return result;
}
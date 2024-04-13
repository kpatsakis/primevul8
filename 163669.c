testcase_resetsolverflags(Solver *solv)
{
  int i;
  for (i = 0; solverflags2str[i].str; i++)
    solver_set_flag(solv, solverflags2str[i].flag, solverflags2str[i].def);
}
set_bound_node_opt_info(NodeOptInfo* opt, MinMaxLen* mmd)
{
  copy_mml(&(opt->exb.mmd),  mmd);
  copy_mml(&(opt->expr.mmd), mmd);
  copy_mml(&(opt->map.mmd),  mmd);
}
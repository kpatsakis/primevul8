alt_merge_node_opt_info(NodeOptInfo* to, NodeOptInfo* add, OptEnv* env)
{
  alt_merge_opt_anc_info  (&to->anc,  &add->anc);
  alt_merge_opt_exact_info(&to->exb,  &add->exb, env);
  alt_merge_opt_exact_info(&to->exm,  &add->exm, env);
  alt_merge_opt_exact_info(&to->expr, &add->expr, env);
  alt_merge_opt_map_info(env->enc, &to->map,  &add->map);

  alt_merge_mml(&to->len, &add->len);
}
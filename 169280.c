cluster_loop_check (struct cluster_list *cluster, struct in_addr originator)
{
  int i;
    
  for (i = 0; i < cluster->length / 4; i++)
    if (cluster->list[i].s_addr == originator.s_addr)
      return 1;
  return 0;
}
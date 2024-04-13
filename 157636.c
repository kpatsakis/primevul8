static void sort_pairing_nodes(FriBidiPairingNode **nodes)
{
  FriBidiPairingNode *front, *back;

  /* 0 or 1 node case */
  if (!*nodes || !(*nodes)->next)
    return;

  pairing_nodes_front_back_split(*nodes, &front, &back);
  sort_pairing_nodes(&front);
  sort_pairing_nodes(&back);
  *nodes = pairing_nodes_sorted_merge(front, back);
}
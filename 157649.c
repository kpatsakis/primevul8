pairing_nodes_sorted_merge(FriBidiPairingNode *nodes1,
                           FriBidiPairingNode *nodes2)
{
  FriBidiPairingNode *res = NULL;
  if (!nodes1)
    return nodes2;
  if (!nodes2)
    return nodes1;

  if (nodes1->open->pos < nodes2->open->pos)
    {
      res = nodes1;
      res->next = pairing_nodes_sorted_merge(nodes1->next, nodes2);
    }
  else
    {
      res = nodes2;
      res->next = pairing_nodes_sorted_merge(nodes1, nodes2->next);
    }
  return res;
}
static void free_pairing_nodes(FriBidiPairingNode *nodes)
{
  while (nodes)
    {
      FriBidiPairingNode *p = nodes;
      nodes = nodes->next;
      fribidi_free(p);
    }
}
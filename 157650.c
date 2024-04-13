static FriBidiPairingNode * pairing_nodes_push(FriBidiPairingNode *nodes,
                                               FriBidiRun *open,
                                               FriBidiRun *close)
{
  FriBidiPairingNode *node = fribidi_malloc(sizeof(FriBidiPairingNode));
  node->open = open;
  node->close = close;
  node->next = nodes;
  nodes = node;
  return nodes;
}
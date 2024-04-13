static void print_pairing_nodes(FriBidiPairingNode *nodes)
{
  MSG ("Pairs: ");
  while (nodes)
    {
      MSG3 ("(%d, %d) ", nodes->open->pos, nodes->close->pos);
      nodes = nodes->next;
    }
  MSG ("\n");
}
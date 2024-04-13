static void pairing_nodes_front_back_split(FriBidiPairingNode *source,
                                           /* output */
                                           FriBidiPairingNode **front,
                                           FriBidiPairingNode **back)
{
  FriBidiPairingNode *pfast, *pslow;
  if (!source || !source->next)
    {
      *front = source;
      *back = NULL;
    }
  else
    {
      pslow = source;
      pfast = source->next;
      while (pfast)
        {
          pfast= pfast->next;
          if (pfast)
            {
              pfast = pfast->next;
              pslow = pslow->next;
            }
        }
      *front = source;
      *back = pslow->next;
      pslow->next = NULL;
    }
}
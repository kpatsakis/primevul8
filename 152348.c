  static void  Ins_GT( INS_ARG )
  { (void)exc;
    if ( args[0] > args[1] )
      args[0] = 1;
    else
      args[0] = 0;
  }
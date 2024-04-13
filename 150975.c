static int propagateConstants(
  Parse *pParse,   /* The parsing context */
  Select *p        /* The query in which to propagate constants */
){
  WhereConst x;
  Walker w;
  int nChng = 0;
  x.db = pParse->db;
  do{
    x.nConst = 0;
    x.nChng = 0;
    x.apExpr = 0;
    findConstInWhere(&x, p->pWhere);
    if( x.nConst ){
      memset(&w, 0, sizeof(w));
      w.pParse = pParse;
      w.xExprCallback = propagateConstantExprRewrite;
      w.xSelectCallback = sqlite3SelectWalkNoop;
      w.xSelectCallback2 = 0;
      w.walkerDepth = 0;
      w.u.pConst = &x;
      sqlite3WalkSelect(&w, p);
      sqlite3DbFree(x.db, x.apExpr);
      nChng += x.nChng;
    }
  }while( x.nChng );  
  return nChng;
}
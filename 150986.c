static void setJoinExpr(Expr *p, int iTable){
  while( p ){
    ExprSetProperty(p, EP_FromJoin);
    assert( !ExprHasProperty(p, EP_TokenOnly|EP_Reduced) );
    ExprSetVVAProperty(p, EP_NoReduce);
    p->iRightJoinTable = (i16)iTable;
    if( p->op==TK_FUNCTION && p->x.pList ){
      int i;
      for(i=0; i<p->x.pList->nExpr; i++){
        setJoinExpr(p->x.pList->a[i].pExpr, iTable);
      }
    }
    setJoinExpr(p->pLeft, iTable);
    p = p->pRight;
  } 
}
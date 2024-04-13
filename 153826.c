int sqlite3ExprCollSeqMatch(Parse *pParse, Expr *pE1, Expr *pE2){
  CollSeq *pColl1 = sqlite3ExprNNCollSeq(pParse, pE1);
  CollSeq *pColl2 = sqlite3ExprNNCollSeq(pParse, pE2);
  return sqlite3StrICmp(pColl1->zName, pColl2->zName)==0;
}
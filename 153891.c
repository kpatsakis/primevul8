void sqlite3ExprCodeGeneratedColumn(
  Parse *pParse,
  Column *pCol,
  int regOut
){
  sqlite3ExprCode(pParse, pCol->pDflt, regOut);
  if( pCol->affinity>=SQLITE_AFF_TEXT ){
    sqlite3VdbeAddOp4(pParse->pVdbe, OP_Affinity, regOut, 1, 0,
                      &pCol->affinity, 1);
  }
}
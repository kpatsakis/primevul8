int sqlite3ExprCodeGetColumn(
  Parse *pParse,   /* Parsing and code generating context */
  Table *pTab,     /* Description of the table we are reading from */
  int iColumn,     /* Index of the table column */
  int iTable,      /* The cursor pointing to the table */
  int iReg,        /* Store results here */
  u8 p5            /* P5 value for OP_Column + FLAGS */
){
  assert( pParse->pVdbe!=0 );
  sqlite3ExprCodeGetColumnOfTable(pParse->pVdbe, pTab, iTable, iColumn, iReg);
  if( p5 ){
    sqlite3VdbeChangeP5(pParse->pVdbe, p5);
  }
  return iReg;
}
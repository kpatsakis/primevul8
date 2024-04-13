void sqlite3ExprCodeGetColumnOfTable(
  Vdbe *v,        /* Parsing context */
  Table *pTab,    /* The table containing the value */
  int iTabCur,    /* The table cursor.  Or the PK cursor for WITHOUT ROWID */
  int iCol,       /* Index of the column to extract */
  int regOut      /* Extract the value into this register */
){
  Column *pCol;
  assert( v!=0 );
  if( pTab==0 ){
    sqlite3VdbeAddOp3(v, OP_Column, iTabCur, iCol, regOut);
    return;
  }
  if( iCol<0 || iCol==pTab->iPKey ){
    sqlite3VdbeAddOp2(v, OP_Rowid, iTabCur, regOut);
  }else{
    int op;
    int x;
    if( IsVirtual(pTab) ){
      op = OP_VColumn;
      x = iCol;
#ifndef SQLITE_OMIT_GENERATED_COLUMNS
    }else if( (pCol = &pTab->aCol[iCol])->colFlags & COLFLAG_VIRTUAL ){
      Parse *pParse = sqlite3VdbeParser(v);
      if( pCol->colFlags & COLFLAG_BUSY ){
        sqlite3ErrorMsg(pParse, "generated column loop on \"%s\"", pCol->zName);
      }else{
        int savedSelfTab = pParse->iSelfTab;
        pCol->colFlags |= COLFLAG_BUSY;
        pParse->iSelfTab = iTabCur+1;
        sqlite3ExprCodeGeneratedColumn(pParse, pCol, regOut);
        pParse->iSelfTab = savedSelfTab;
        pCol->colFlags &= ~COLFLAG_BUSY;
      }
      return;
#endif
    }else if( !HasRowid(pTab) ){
      testcase( iCol!=sqlite3TableColumnToStorage(pTab, iCol) );
      x = sqlite3TableColumnToIndex(sqlite3PrimaryKeyIndex(pTab), iCol);
      op = OP_Column;
    }else{
      x = sqlite3TableColumnToStorage(pTab,iCol);
      testcase( x!=iCol );
      op = OP_Column;
    }
    sqlite3VdbeAddOp3(v, op, iTabCur, x, regOut);
    sqlite3ColumnDefault(v, pTab, iCol, regOut);
  }
}
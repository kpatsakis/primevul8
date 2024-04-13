static void codeInteger(Parse *pParse, Expr *pExpr, int negFlag, int iMem){
  Vdbe *v = pParse->pVdbe;
  if( pExpr->flags & EP_IntValue ){
    int i = pExpr->u.iValue;
    assert( i>=0 );
    if( negFlag ) i = -i;
    sqlite3VdbeAddOp2(v, OP_Integer, i, iMem);
  }else{
    int c;
    i64 value;
    const char *z = pExpr->u.zToken;
    assert( z!=0 );
    c = sqlite3DecOrHexToI64(z, &value);
    if( (c==3 && !negFlag) || (c==2) || (negFlag && value==SMALLEST_INT64)){
#ifdef SQLITE_OMIT_FLOATING_POINT
      sqlite3ErrorMsg(pParse, "oversized integer: %s%s", negFlag ? "-" : "", z);
#else
#ifndef SQLITE_OMIT_HEX_INTEGER
      if( sqlite3_strnicmp(z,"0x",2)==0 ){
        sqlite3ErrorMsg(pParse, "hex literal too big: %s%s", negFlag?"-":"",z);
      }else
#endif
      {
        codeReal(v, z, negFlag, iMem);
      }
#endif
    }else{
      if( negFlag ){ value = c==3 ? SMALLEST_INT64 : -value; }
      sqlite3VdbeAddOp4Dup8(v, OP_Int64, 0, iMem, 0, (u8*)&value, P4_INT64);
    }
  }
}
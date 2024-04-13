static void codeReal(Vdbe *v, const char *z, int negateFlag, int iMem){
  if( ALWAYS(z!=0) ){
    double value;
    sqlite3AtoF(z, &value, sqlite3Strlen30(z), SQLITE_UTF8);
    assert( !sqlite3IsNaN(value) ); /* The new AtoF never returns NaN */
    if( negateFlag ) value = -value;
    sqlite3VdbeAddOp4Dup8(v, OP_Real, 0, iMem, 0, (u8*)&value, P4_REAL);
  }
}
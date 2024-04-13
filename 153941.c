static void codeApplyAffinity(Parse *pParse, int base, int n, char *zAff){
  Vdbe *v = pParse->pVdbe;
  if( zAff==0 ){
    assert( pParse->db->mallocFailed );
    return;
  }
  assert( v!=0 );

  /* Adjust base and n to skip over SQLITE_AFF_BLOB and SQLITE_AFF_NONE
  ** entries at the beginning and end of the affinity string.
  */
  assert( SQLITE_AFF_NONE<SQLITE_AFF_BLOB );
  while( n>0 && zAff[0]<=SQLITE_AFF_BLOB ){
    n--;
    base++;
    zAff++;
  }
  while( n>1 && zAff[n-1]<=SQLITE_AFF_BLOB ){
    n--;
  }

  /* Code the OP_Affinity opcode if there is anything left to do. */
  if( n>0 ){
    sqlite3VdbeAddOp4(v, OP_Affinity, base, n, 0, zAff, n);
  }
}
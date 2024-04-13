static void sqlite3SetHasNullFlag(Vdbe *v, int iCur, int regHasNull){
  int addr1;
  sqlite3VdbeAddOp2(v, OP_Integer, 0, regHasNull);
  addr1 = sqlite3VdbeAddOp1(v, OP_Rewind, iCur); VdbeCoverage(v);
  sqlite3VdbeAddOp3(v, OP_Column, iCur, 0, regHasNull);
  sqlite3VdbeChangeP5(v, OPFLAG_TYPEOFARG);
  VdbeComment((v, "first_entry_in(%d)", iCur));
  sqlite3VdbeJumpHere(v, addr1);
}
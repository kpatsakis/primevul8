nv_gd(
    oparg_T	*oap,
    int		nchar,
    int		thisblock)	// 1 for "1gd" and "1gD"
{
    int		len;
    char_u	*ptr;

    if ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0
	    || find_decl(ptr, len, nchar == 'd', thisblock, SEARCH_START)
								      == FAIL)
	clearopbeep(oap);
#ifdef FEAT_FOLDING
    else if ((fdo_flags & FDO_SEARCH) && KeyTyped && oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
}
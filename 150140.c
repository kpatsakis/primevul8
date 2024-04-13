stack_type_str(int stack_type)
{
  switch (stack_type) {
    case STK_ALT:		return "Alt   ";
    case STK_LOOK_BEHIND_NOT:	return "LBNot ";
    case STK_POS_NOT:		return "PosNot";
    case STK_MEM_START:		return "MemS  ";
    case STK_MEM_END:		return "MemE  ";
    case STK_REPEAT_INC:	return "RepInc";
    case STK_STATE_CHECK_MARK:	return "StChMk";
    case STK_NULL_CHECK_START:	return "NulChS";
    case STK_NULL_CHECK_END:	return "NulChE";
    case STK_MEM_END_MARK:	return "MemEMk";
    case STK_POS:		return "Pos   ";
    case STK_STOP_BT:		return "StopBt";
    case STK_REPEAT:		return "Rep   ";
    case STK_CALL_FRAME:	return "Call  ";
    case STK_RETURN:		return "Ret   ";
    case STK_VOID:		return "Void  ";
    default:			return "      ";
  }
}
node_new_callout(Node** node, OnigCalloutOf callout_of, int num, int id,
                 ScanEnv* env)
{
  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->id          = id;
  GIMMICK_(*node)->num         = num;
  GIMMICK_(*node)->type        = GIMMICK_CALLOUT;
  GIMMICK_(*node)->detail_type = (int )callout_of;

  return ONIG_NORMAL;
}
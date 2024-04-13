VariationalRefinementImpl::ComputeSmoothnessTermHorPass_ParBody::ComputeSmoothnessTermHorPass_ParBody(
  VariationalRefinementImpl &_var, int _nstripes, int _h, RedBlackBuffer &_W_u, RedBlackBuffer &_W_v,
  RedBlackBuffer &_tempW_u, RedBlackBuffer &_tempW_v, bool _red_pass)
    : var(&_var), nstripes(_nstripes), h(_h), W_u(&_W_u), W_v(&_W_v), curW_u(&_tempW_u), curW_v(&_tempW_v),
      red_pass(_red_pass)
{
    stripe_sz = (int)ceil(h / (double)nstripes);
}
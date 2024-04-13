VariationalRefinementImpl::ComputeSmoothnessTermVertPass_ParBody::ComputeSmoothnessTermVertPass_ParBody(
  VariationalRefinementImpl &_var, int _nstripes, int _h, RedBlackBuffer &_W_u, RedBlackBuffer &_W_v, bool _red_pass)
    : var(&_var), nstripes(_nstripes), W_u(&_W_u), W_v(&_W_v), red_pass(_red_pass)
{
    /* Omit the last row in the vertical pass */
    h = _h - 1;
    stripe_sz = (int)ceil(h / (double)nstripes);
}
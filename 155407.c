int CephxSessionHandler::sign_message(Message *m)
{
  // If runtime signing option is off, just return success without signing.
  if (!cct->_conf->cephx_sign_messages) {
    return 0;
  }

  uint64_t sig;
  int r = _calc_signature(m, &sig);
  if (r < 0)
    return r;

  ceph_msg_footer& f = m->get_footer();
  f.sig = sig;
  f.flags = (unsigned)f.flags | CEPH_MSG_FOOTER_SIGNED;
  ldout(cct, 20) << "Putting signature in client message(seq # " << m->get_seq()
		 << "): sig = " << sig << dendl;
  return 0;
}
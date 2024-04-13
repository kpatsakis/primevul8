int CephxSessionHandler::check_message_signature(Message *m)
{
  // If runtime signing option is off, just return success without checking signature.
  if (!cct->_conf->cephx_sign_messages) {
    return 0;
  }
  if ((features & CEPH_FEATURE_MSG_AUTH) == 0) {
    // it's fine, we didn't negotiate this feature.
    return 0;
  }

  uint64_t sig;
  int r = _calc_signature(m, &sig);
  if (r < 0)
    return r;

  if (sig != m->get_footer().sig) {
    // Should have been signed, but signature check failed.  PLR
    if (!(m->get_footer().flags & CEPH_MSG_FOOTER_SIGNED)) {
      ldout(cct, 0) << "SIGN: MSG " << m->get_seq() << " Sender did not set CEPH_MSG_FOOTER_SIGNED." << dendl;
    }
    ldout(cct, 0) << "SIGN: MSG " << m->get_seq() << " Message signature does not match contents." << dendl;
    ldout(cct, 0) << "SIGN: MSG " << m->get_seq() << "Signature on message:" << dendl;
    ldout(cct, 0) << "SIGN: MSG " << m->get_seq() << "    sig: " << m->get_footer().sig << dendl;
    ldout(cct, 0) << "SIGN: MSG " << m->get_seq() << "Locally calculated signature:" << dendl;
    ldout(cct, 0) << "SIGN: MSG " << m->get_seq() << "    sig_check:" << sig << dendl;

    // For the moment, printing an error message to the log and
    // returning failure is sufficient.  In the long term, we should
    // probably have code parsing the log looking for this kind of
    // security failure, particularly when there are large numbers of
    // them, since the latter is a potential sign of an attack.  PLR

    ldout(cct, 0) << "Signature failed." << dendl;
    return (SESSION_SIGNATURE_FAILURE);
  }

  return 0;
}
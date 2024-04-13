Opal::Call::OnCleared ()
{
  std::string reason;

  NoAnswerTimer.Stop (false);

  // TODO find a better way than that
  while (!call_setup)
    PThread::Current ()->Sleep (100);

  if (!IsEstablished ()
      && !is_outgoing ()
      && GetCallEndReason () != OpalConnection::EndedByAnswerDenied) {

    Ekiga::Runtime::run_in_main (boost::bind (&Opal::Call::emit_missed_in_main, this));
  }
  else {

    switch (GetCallEndReason ()) {

    case OpalConnection::EndedByLocalUser :
      reason = _("Local user cleared the call");
      break;
    case OpalConnection::EndedByNoAccept :
      reason = _("Local user rejected the call");
      break;
    case OpalConnection::EndedByAnswerDenied :
      reason = _("Local user rejected the call");
      break;
    case OpalConnection::EndedByRemoteUser :
      reason = _("Remote user cleared the call");
      break;
    case OpalConnection::EndedByRefusal :
      reason = _("Remote user rejected the call");
      break;
    case OpalConnection::EndedByCallerAbort :
      reason = _("Remote user has stopped calling");
      break;
    case OpalConnection::EndedByTransportFail :
      reason = _("Abnormal call termination");
      break;
    case OpalConnection::EndedByConnectFail :
      reason = _("Could not connect to remote host");
      break;
    case OpalConnection::EndedByGatekeeper :
    case OpalConnection::EndedByGkAdmissionFailed :
      reason = _("The Gatekeeper cleared the call");
      break;
    case OpalConnection::EndedByNoUser :
      reason = _("User not found");
      break;
    case OpalConnection::EndedByNoBandwidth :
      reason = _("Insufficient bandwidth");
      break;
    case OpalConnection::EndedByCapabilityExchange :
      reason = _("No common codec");
      break;
    case OpalConnection::EndedByCallForwarded :
      reason = _("Call forwarded");
      break;
    case OpalConnection::EndedBySecurityDenial :
      reason = _("Security check failed");
      break;
    case OpalConnection::EndedByLocalBusy :
      reason = _("Local user is busy");
      break;
    case OpalConnection::EndedByLocalCongestion :
      reason = _("Congested link to remote party");
      break;
    case OpalConnection::EndedByRemoteBusy :
      reason = _("Remote user is busy");
      break;
    case OpalConnection::EndedByRemoteCongestion :
      reason = _("Congested link to remote party");
      break;
    case OpalConnection::EndedByHostOffline :
      reason = _("Remote host is offline");
      break;
    case OpalConnection::EndedByTemporaryFailure :
    case OpalConnection::EndedByUnreachable :
    case OpalConnection::EndedByNoEndPoint :
    case OpalConnection::EndedByNoAnswer :
      reason = _("User is not available");
      break;
    case OpalConnection::EndedByOutOfService:
      reason = _("Service unavailable");  // this appears when 500 does not work
      break;
    case OpalConnection::EndedByQ931Cause:
    case OpalConnection::EndedByDurationLimit:
    case OpalConnection::EndedByInvalidConferenceID:
    case OpalConnection::EndedByNoDialTone:
    case OpalConnection::EndedByNoRingBackTone:
    case OpalConnection::EndedByAcceptingCallWaiting:
    case OpalConnection::NumCallEndReasons:
    default :
      reason = _("Call completed");
    }

    Ekiga::Runtime::run_in_main (boost::bind (&Opal::Call::emit_cleared_in_main, this, reason));
  }
}
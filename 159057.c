Opal::Call::Call (OpalManager& _manager,
		  Ekiga::ServiceCore& _core,
		  const std::string& uri)
  : OpalCall (_manager), Ekiga::Call (), core (_core), remote_uri (uri),
    call_setup(false), jitter(0), outgoing(false)
{
  notification_core = core.get<Ekiga::NotificationCore> ("notification-core");
  re_a_bytes = tr_a_bytes = re_v_bytes = tr_v_bytes = 0.0;
  last_v_tick = last_a_tick = PTime ();
  total_a =
    total_v =
    lost_a =
    too_late_a =
    out_of_order_a =
    lost_v =
    too_late_v =
    out_of_order_v = 0;
  lost_packets = late_packets = out_of_order_packets = 0.0;
  re_a_bw = tr_a_bw = re_v_bw = tr_v_bw = 0.0;

  NoAnswerTimer.SetNotifier (PCREATE_NOTIFIER (OnNoAnswerTimeout));
}
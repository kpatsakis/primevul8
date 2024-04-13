Opal::Call::OnRTPStatistics (const OpalConnection & /* connection */,
			     const RTP_Session & session)
{
  PWaitAndSignal m(stats_mutex); // The stats are computed from two different threads

  if (session.IsAudio ()) {

    PTimeInterval t = PTime () - last_a_tick;
    if (t.GetMilliSeconds () < 500)
      return;

    unsigned elapsed_seconds = max ((unsigned long) t.GetMilliSeconds (), (unsigned long) 1);
    double octets_received = session.GetOctetsReceived ();
    double octets_sent = session.GetOctetsSent ();

    re_a_bw = max ((octets_received - re_a_bytes) / elapsed_seconds, 0.0);
    tr_a_bw = max ((octets_sent - tr_a_bytes) / elapsed_seconds, 0.0);

    re_a_bytes = octets_received;
    tr_a_bytes = octets_sent;
    last_a_tick = PTime ();

    total_a = session.GetPacketsReceived ();
    lost_a = session.GetPacketsLost ();
    too_late_a = session.GetPacketsTooLate ();
    out_of_order_a = session.GetPacketsOutOfOrder ();

    jitter = session.GetJitterBufferSize () / max ((unsigned) session.GetJitterTimeUnits (), (unsigned) 8);
  }
  else {

    PTimeInterval t = PTime () - last_v_tick;
    if (t.GetMilliSeconds () < 500)
      return;

    unsigned elapsed_seconds = max ((unsigned long) t.GetMilliSeconds (), (unsigned long) 1);
    double octets_received = session.GetOctetsReceived ();
    double octets_sent = session.GetOctetsSent ();

    re_v_bw = max ((octets_received - re_v_bytes) / elapsed_seconds, 0.0);
    tr_v_bw = max ((octets_sent - tr_v_bytes) / elapsed_seconds, 0.0);

    re_v_bytes = octets_received;
    tr_v_bytes = octets_sent;
    last_v_tick = PTime ();

    total_v = session.GetPacketsReceived ();
    lost_v = session.GetPacketsLost ();
    too_late_v = session.GetPacketsTooLate ();
    out_of_order_v = session.GetPacketsOutOfOrder ();
  }

  lost_packets = (lost_a + lost_v) / max ((unsigned long)(total_a + total_v), (unsigned long) 1);
  late_packets = (too_late_a + too_late_v) / max ((unsigned long)(total_a + total_v), (unsigned long) 1);
  out_of_order_packets = (out_of_order_a + out_of_order_v) / max ((unsigned long)(total_a + total_v), (unsigned long) 1);
}
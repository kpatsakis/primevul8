Opal::Call::emit_missed_in_main ()
{
  boost::shared_ptr<Ekiga::CallCore> call_core = core.get<Ekiga::CallCore> ("call-core");
  std::stringstream msg;

  missed ();
  msg << _("Missed call from") << " " << get_remote_party_name ();
  boost::shared_ptr<Ekiga::Notification> notif (new Ekiga::Notification (Ekiga::Notification::Warning,
                                                                         _("Missed call"), msg.str (),
                                                                         _("Call"),
                                                                         boost::bind (&Ekiga::CallCore::dial, call_core,
                                                                                      get_remote_uri ())));
  notification_core->push_notification (notif);
}
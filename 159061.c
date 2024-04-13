  CallSetup (Opal::Call & _call,
             OpalConnection & _connection)
    : PThread (1000, AutoDeleteThread),
      call (_call),
      connection (_connection)
  {
    this->Resume ();
  }
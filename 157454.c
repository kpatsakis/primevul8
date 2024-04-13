rfbNewClient(rfbScreenInfoPtr rfbScreen,
             int sock)
{
  return(rfbNewTCPOrUDPClient(rfbScreen,sock,FALSE));
}
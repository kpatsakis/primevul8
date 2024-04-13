rfbNewUDPConnection(rfbScreenInfoPtr rfbScreen,
                    int sock)
{
  if (write(sock, (char*) &ptrAcceleration, 1) < 0) {
	rfbLogPerror("rfbNewUDPConnection: write");
    }
}
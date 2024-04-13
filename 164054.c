void RemoteFsDevice::toggle()
{
    if (isConnected()) {
        stopScanner();
        unmount();
    } else {
        mount();
    }
}
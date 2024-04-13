void RemoteFsDevice::Details::save() const
{
    Configuration cfg(constCfgPrefix+name);

    cfg.set("url", url.toString());
    cfg.set("extraOptions", extraOptions);
    cfg.set("configured", configured);
}
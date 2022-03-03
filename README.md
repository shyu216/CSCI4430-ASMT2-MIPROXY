# code在miProxy里，能写一点是一点吧，我先写main。。。

### share files from vmware in xubuntu 
```
sudo vim /etc/fstab
.host:/    /mnt/hgfs    vmhgfs    defaults    0    0
```

### connect internet
``` 
ip link
sudo ip link set ens33 up
sudo dhclient ens33 -v
```

### change time to enable flash
```
sudo date +%Y%m%d -s 20180101
```

### useful link
https://github.com/mgild/Networkp2.git  
https://github.com/heaventourist/Video-Streaming-via-CDN.git
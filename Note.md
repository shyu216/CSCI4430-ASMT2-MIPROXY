### xubuntu share files in vmware
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
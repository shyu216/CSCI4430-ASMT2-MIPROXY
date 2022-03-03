# code在miProxy里，能写一点是一点吧，我先写main。。。现在写handler。。。

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

### firefox header
```
Message GET /index.html HTTP/1.1
Host: 127.0.0.1:8888
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:59.0) Gecko/20100101 Firefox/59.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Cache-Control: max-age=0
```
```
Accept	
text/html,application/xhtml+xm…plication/xml;q=0.9,*/*;q=0.8
Accept-Encoding	
gzip, deflate
Accept-Language	
en-US,en;q=0.5
Connection	
keep-alive
Host	
localhost
Referer	
http://localhost/StrobeMediaPlayback.swf
User-Agent	
Mozilla/5.0 (X11; Ubuntu; Linu…) Gecko/20100101 Firefox/59.0
```

### server header
```
Connection	
Keep-Alive
Content-Length	
182637
Content-Type	
video/f4f
Date	
Thu, 03 Mar 2022 12:42:15 GMT
Keep-Alive	
timeout=30000, max=499988
Last-Modified	
Mon, 21 Sep 2020 23:08:23 GMT
Server	
Apache/2.2.32 (Unix)
```



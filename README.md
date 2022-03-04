# code在miProxy里,建好proxy的client和server了，下一步parse the http header。。。

<img src="ss.png" title="firefox" alt=""/>  

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
GET /index.html HTTP/1.1
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

### server response index.html
```html
HTTP/1.1 200 OK
Date: Mon, 01 Jan 2018 00:35:17 GMT
Server: Apache/2.2.32 (Unix)
Last-Modified: Mon, 01 Jan 2018 00:35:17 GMT
ETag: W/"49618-764-5afdaed4c2c8b"
Accept-Ranges: bytes
Content-Length: 1892
Keep-Alive: timeout=30000, max=500000
Connection: Keep-Alive
Content-Type: text/html

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Strobe Media Playback</title>
    <script type="text/javascript" src="swfobject.js"></script>
	<script type="text/javascript">  		
        var loc = window.location.host

    	// Create a StrobeMediaPlayback configuration
		var parameters =
			{	src: "http://"+loc+"/vod/big_buck_bunny.f4m"
   			,	autoPlay: true
			,	controlBarAutoHide: false
			,   javascriptCallbackFunction: "onJavaScriptBridgeCreated"
			};
    		
		// Embed the player SWF:
		swfobject.embedSWF
			( "StrobeMediaPlayback.swf"
			, "strobeMediaPlayback"
			, 1280
			, 720
			, "10.1.0"
			, {}
			, parameters
			, { allowFullScreen: "true"}
			, { name: "strobeMediaPlayback" }
			);

			
		function onCurrentTimeChange(time, playerId)
		{
			document.getElementById("currentTime").innerHTML = time;
		}
		
		function onDurationChange(time, playerId)
		{
			document.getElementById("duration").innerHTML = time;
		}
		var player = null;
		function onJavaScriptBridgeCreated(playerId)
		{
			if (player == null) {
				player = document.getElementById(playerId);
				
				// Add event listeners that will update the
				player.addEventListener("currentTimeChange", "onCurrentTimeChange");
				player.addEventListener("durationChange", "onDurationChange");
				
				// Pause/Resume the playback when we click the Play/Pause link
				document.getElementById("play-pause").onclick = function(){
					var state = player.getState();
					if (state == "ready" || state == "paused") {
						player.play2();
					}
					else
						if (state == "playing") {
							player.pause();
						}
					return false;
				};
			}
		}
    </script>  	
  </head>
  <body>
	<div id="strobeMediaPlayback">
      <p>Alternative content</p>
    </div>
  </body>
</html>
```


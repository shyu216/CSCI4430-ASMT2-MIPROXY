# What Ta says

### 175
One chunk here is a <bitrate>Seg<x>-Frag<y> file.
  

### 187
The number of segments/fragments is measured in the test case. However, as long as the number of segments is not too small (which means your miproxy may be inefficient), you should be able to pass the testcase.

### 178
All the three tput values are used to calculate the avg-tput.

### 172
- In the same connection to the server, the server will response in sequence. For example, your proxy connects to the server in one connection and sends request 1 anf request 2 in order. Then, you server will send response 1 and response 2 in order as well.

- For the same client, it will wait for the completion of receiving the packet from server. For a different client from the one that is receiving the packet, it is possible that it sends a request while the server is sending the packet to another client halfway.

### 167
You can assume no more than 8 concurrent connections in HW2.

### 165
you do not need to consider the situation that more than one tabs are playing the video for one browser profile.

### 162
In this case, the chunk name is "1000Seg100-Frag583” instead of the full path one. (log)

### 159
In this assignment, you can assume that the HTTP header should be no more than 1024 bytes.

### 157
- For the XML parser, you can just find out the <media> element in the f4m file. Then, you can parse the bitrate attribute of the element, from which you can get all the available bitrates.

- For the XML parsing, you do not need to parse all the elements of the file. You only need to parse the <media> element and get the bitrate attribute from it. Thus, implementing a XML parser that will only parse the"bitrate" from <media> element will not be very difficult.

- Some hints on the parser:

- You can use the strstr function to get the location of <media> element in the string. Then, you can find the location for bitrate from where the found <media> element starts. Then, you find the next <media> element and repeat the steps to get the bitrates util there is no <media> element left.
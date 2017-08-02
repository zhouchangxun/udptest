# udptest
udp health check test.

# NOTE
如果端口或主机不可达，那么icmp错误信息是在向服务程序发送数据后收到的，而不是在调用connect()时。
如果你使用tcpdump程序来抓包，会发现收到的是ICMP的错误信息。
```
tcpdump -n -i any icmp or port 8888
```
when port unrechable:
11:15:42.458701 IP 10.157.5.233 > 10.157.5.233: ICMP 10.157.5.233 udp port 8888 unreachable, length 40 
when host unrechable:
10:50:43.089484 IP 10.157.5.233 > 10.157.5.233: ICMP host 10.157.5.239 unreachable, length 37

如果icmp被禁了的话，是没有办法知道udp服务端口是否开启的.

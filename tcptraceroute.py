import os
import sys
import struct
import socket

def trace():
    icmp = socket.getprotobyname('icmp')
    tcp = socket.getprotobyname('tcp')
    ttl=1
    host,port=sys.argv[1:]
    port=int(port)
    host_addr=socket.gethostbyname(host)
    for ttl in range(1,31):
        recv_socket =socket.socket(socket.AF_INET,socket.SOCK_RAW,icmp)
        send_socket=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        send_socket.setsockopt(socket.IPPROTO_IP, socket.IP_TTL, struct.pack('I', ttl))
        send_socket.settimeout(2)
        timeout=struct.pack("ll",5,0)
        recv_socket.setsockopt(socket.SOL_SOCKET,socket.SO_RCVTIMEO,timeout)
        recv_socket.bind(("",port))
        try:
            send_socket.connect((host,port))
        except (socket.error, socket.timeout), err:
            curr_addr = None
            curr_name = None
            try:
                _,curr_addr = recv_socket.recvfrom(512)
                curr_addr = curr_addr[0]
                try:
                    curr_name = socket.gethostbyaddr(curr_addr)[0]
                except socket.error:
                    curr_name= curr_addr
            except socket.error:
                pass
            finally:
                send_socket.close()
                recv_socket.close()

            if curr_addr is not None:
                curr_host = "%s (%s)" % ( curr_name, curr_addr)
            else:
                curr_host = "*"
            print(("%d\t%s") % (ttl, curr_host))
            
            if curr_addr == host_addr:
                break
            continue
        except KeyboardInterrupt:
            print 'ttl=%02d (KeyboardInterrupt)' % ttl
            break
trace()


import socket
import time
import threading
import sys
import os
#import multiprocessing
from excel_iot import excel_put,excel_check

#n=3            number of sockets to be opened

def multisockets(i):
	while(True):
	
		try:
		    	
			line=str(readlines(sockets["%s"%i]))
			  
			if len(line)==7 or len(line)==8:      #put beacons in database
					print(line)
					lock.acquire()
					excel_put(line)
					
					#sockets["%s"%i].sendall(a.encode('utf-8')) 
					lock.release()
					
			if len(line)==14 or len(line)==15:                   #check received beacon in database 
				print(line)
				lock.acquire()
				
				x=excel_check(line[7:])
				
				if x=="found":
				
					if len(line)==14:
						data.setdefault("%s:%s"%(i,line[7]), []).append(line[7:])    #x:y where x is the receiving node and y denotes the received's
						print("NODE %s:"%i)	
						for key,value in data.iteritems():
							if key[0]=="%s"%i and key[1]==":":
							    print(key,value)
							    if len(value)==3 and str(value[0][0])==line[7]:
								print("sending back 3 beacons matched with Node : %s"%line[7])
							    	sockets["%s"%i].sendall("%s0%s".encode('utf-8') % (a,line[7])) 
							if key[0:2]=="%s"%i:
							    print(key,value)
							    if len(value)==3 and str(value[0][0])==line[7]:
								print("sending back 3 beacons matched with Node : %s"%line[7])
								sockets["%s"%i].sendall("%s0%s".encode('utf-8') % (a,line[7])) 
						    	
					if len(line)==15:
				         data.setdefault("%s:%s"%(i,line[7:9]), []).append(line[7:])   
				         #l=[value  if key[0]=="%s"%i and key[1]==":" else if key[0:2]=="%s"%i for key,value in data.iteritems() ]
				         for key,value in data.iteritems():
				         	if key[0]=="%s"%i and key[1]==":":
						    print(key,value)
						    if len(value)==3 and str(value[0][0:2])==line[7:9]:
							print("sending back 3 beacons matched with Node : %s"%line[7:9])
						    	sockets["%s"%i].sendall("%s%s".encode('utf-8') % (a,line[7:9])) 
						if key[0:2]=="%s"%i:
						    print(key,value)
						    if len(value)==3 and str(value[0][0:2])==line[7:9]:
							print("sending back 3 beacons matched with Node : %s"%line[7:9])
							sockets["%s"%i].sendall("%s%s".encode('utf-8') % (a,line[7:9])) 
						
				lock.release()	
		except Exception as e:
			print(e)
			break

def readlines(socket):
   line = ""
   while True:
    data = socket.recv(1).decode('utf-8')
    
    if data != "\n":
            line+=data
    else:
            break
    
   return line
   			
def run():
	global sockets,lock,a,b,data,all			
	try:
	 os.remove("%s/MOTES_DATABASE/BEACONS.xlsx" % os.getcwd())
	except Exception as e:
	 print(e) 
	print("-------removed already stored database file(optional)")

	   
	HOST = "127.0.0.1"  # The server's hostname or IP address
	PORT = 60001 # The port used by the server



	a="Contact is made"
	b="No contact seen"   
	
	sockets={}
        data={}
        all=[]
	n=1
	

	lock=threading.Lock()

	while(True):
	  try:
	   #print(i)
	   sockets["%s"%(n)]=socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
	   sockets["%s"%(n)].connect((HOST,60000+n))
	   print("connected socket %s----"%n)
	   print("n:",n) 
	   n+=1
	   
	  except Exception as e:
	   print(e,"from socket",n)
	   #sys.exit(0)
	   n-=1
	   break
	   
        print(n)
	Threads={}   
	for i in range(1,n+1):
	  
	  Threads["%s"%i]=threading.Thread(target=multisockets, args=(i,))
	print("socket %s joined and started"%i)   
	#print(Threads)
	for i in range(1,n+1):  
	  Threads["%s"%i].start()
	  
	for i in range(1,n+1):  
	  Threads["%s"%i].join()
  
if __name__=='__main__':
  run()
    
  
   



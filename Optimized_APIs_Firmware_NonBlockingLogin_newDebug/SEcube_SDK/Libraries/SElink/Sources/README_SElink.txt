SElink(tm)


To install SElink(tm):


---On Server-side (Linux environment)
1) Install prerequisites (libboost)
2) cd proxy/
3) make
4) cd bin/
5) launch service ./selinkgw with desired configuration
e.g.:
./selinkgw -c:selinkgw.json -p soft -k:keys.json -f
./selinkgw -c:selinkgw.json -p secube -d _devSN_ -z _pin_ -f
./selinkgw -l logfile -c:selinkgw.json -p secube -d 0102030405060708090A0D0C0D0E0F100102030405060708090A0D0C0D0E0F10 -z pin -f

Note: 
_devSN_ should be a 32 byte string with the serial number of the device in hex format;
Also, for _pin_ but it has to be written into a file

For help launch:
./selinkgw -h

For configuration examples, see:
/proxy/example



---On Client-side (Windows 10)
1) Disable check on drivers signature (i.e. enable "TestMode")
2) Install /installer/selink-setup.exe
3) Configure .ini (file Programs/SElink/selinksvc.ini) with:
   provider=soft
   keys=:keys.json
for encryption without a SEcube device, or
   provider=secube
for encryption with a SEcube device
4) Open SElink GUI
5) Connect to the SEcube device
5) Set rules for desired applications

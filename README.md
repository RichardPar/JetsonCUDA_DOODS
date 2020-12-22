# JetsonCUDA_DOODS

an RPC that Mimics the DOODS RPC method of getting inference data. This is a Proof of Concept<br>
Based on this AWESOME project<br>
https://github.com/snowzach/doods/
<br>

# NOTES: 
* I am not a C++ developer! this is my first one!
* mkstmp is used, its insecure as per warnings; but this is a proof of concept
* Error checking is minimal!

The Code is build on a Jetson Nano with JETPACK installed.


Dependencies<br>
nvidia JetPack<br>
rapidjson<br>
https://github.com/Tencent/rapidjson/<br>

pistache<br> 
http://pistache.io/quickstart<br>

Download and install accordingly

Port is defaulted to 8080 and is hardcoded in source... change to what you desire if needed. As the detectNet uses Nvidia command line processing, you can use --NETWORK=xxxxxxxx and run against a downloaded/custom CNN dataset

Valid Commands are -
GET
  /version
  /detectors
POST
  /detect

# To Build
Git Clone/Unpack....<br>
cd <where you downloaded><br>
mkdir build<br>
cd build<br>
  cmake ../<br>
  make<br>
  



#To TEST

echo "{\"detector_name\":\"default\", \"detect\":{\"*\":60}, \"data\":\"`cat peds_2.png|base64 -w0`\"}" > /tmp/postdata.json && time curl -v -H "Expect:"  -d@/tmp/postdata.json -H "Content-Type: application/json" -X POST http://localhost:8080/detect

*   Trying 192.168.2.58...
* TCP_NODELAY set
* Connected to jetson.local (192.168.2.58) port 8080 (#0)
> POST /detect HTTP/1.1
> Host: jetson.local:8080
> User-Agent: curl/7.58.0
> Accept: */*
> Content-Type: application/json
> Content-Length: 501565
> 
* We are completely uploaded and fine
< HTTP/1.1 200 OK
< Connection: Close
< Content-Length: 362
< 
* Closing connection 0
{"detections":[{"top":0.3072866,"left":0.0308209,"bottom":0.9891333,"right":0.4535146,"label":"motorcycle","confidence":76.629364},{"top":0.2861912,"left":0.7390475,"bottom":0.7178121,"right":0.8755167,"label":"person","confidence":93.0836486},{"top":0.3354573,"left":0.4695544,"bottom":0.5971207,"right":0.6403077,"label":"motorcycle","confidence":88.0180511}]}<br>

real	0m0.276s<br>
user	0m0.004s<br>
sys	0m0.020s<br>


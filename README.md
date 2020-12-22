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
  



# To TEST

```
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
```
# Output 

```
./cnnserver 

detectNet -- loading detection network model from:
          -- model        networks/SSD-Mobilenet-v2/ssd_mobilenet_v2_coco.uff
          -- input_blob   'Input'
          -- output_blob  'NMS'
          -- output_count 'NMS_1'
          -- class_labels networks/SSD-Mobilenet-v2/ssd_coco_labels.txt
          -- threshold    0.500000
          -- batch_size   1

[TRT]    TensorRT version 7.1.3
[TRT]    loading NVIDIA plugins...
[TRT]    Registered plugin creator - ::GridAnchor_TRT version 1
[TRT]    Registered plugin creator - ::NMS_TRT version 1
[TRT]    Registered plugin creator - ::Reorg_TRT version 1
[TRT]    Registered plugin creator - ::Region_TRT version 1
[TRT]    Registered plugin creator - ::Clip_TRT version 1
[TRT]    Registered plugin creator - ::LReLU_TRT version 1
[TRT]    Registered plugin creator - ::PriorBox_TRT version 1
[TRT]    Registered plugin creator - ::Normalize_TRT version 1
[TRT]    Registered plugin creator - ::RPROI_TRT version 1
[TRT]    Registered plugin creator - ::BatchedNMS_TRT version 1
[TRT]    Could not register plugin creator -  ::FlattenConcat_TRT version 1
[TRT]    Registered plugin creator - ::CropAndResize version 1
[TRT]    Registered plugin creator - ::DetectionLayer_TRT version 1
[TRT]    Registered plugin creator - ::Proposal version 1
[TRT]    Registered plugin creator - ::ProposalLayer_TRT version 1
[TRT]    Registered plugin creator - ::PyramidROIAlign_TRT version 1
[TRT]    Registered plugin creator - ::ResizeNearest_TRT version 1
[TRT]    Registered plugin creator - ::Split version 1
[TRT]    Registered plugin creator - ::SpecialSlice_TRT version 1
[TRT]    Registered plugin creator - ::InstanceNormalization_TRT version 1
[TRT]    detected model format - UFF  (extension '.uff')
[TRT]    desired precision specified for GPU: FASTEST
[TRT]    requested fasted precision for device GPU without providing valid calibrator, disabling INT8
[TRT]    native precisions detected for GPU:  FP32, FP16
[TRT]    selecting fastest native precision for GPU:  FP16
[TRT]    attempting to open engine cache file /usr/local/bin/networks/SSD-Mobilenet-v2/ssd_mobilenet_v2_coco.uff.1.1.7103.GPU.FP16.engine
[TRT]    loading network plan from engine cache... /usr/local/bin/networks/SSD-Mobilenet-v2/ssd_mobilenet_v2_coco.uff.1.1.7103.GPU.FP16.engine
[TRT]    device GPU, loaded /usr/local/bin/networks/SSD-Mobilenet-v2/ssd_mobilenet_v2_coco.uff
[TRT]    Deserialize required 3105481 microseconds.
[TRT]    
[TRT]    CUDA engine context initialized on device GPU:
[TRT]       -- layers       116
[TRT]       -- maxBatchSize 1
[TRT]       -- workspace    0
[TRT]       -- deviceMemory 35449856
[TRT]       -- bindings     3
[TRT]       binding 0
                -- index   0
                -- name    'Input'
                -- type    FP32
                -- in/out  INPUT
                -- # dims  3
                -- dim #0  3 (SPATIAL)
                -- dim #1  300 (SPATIAL)
                -- dim #2  300 (SPATIAL)
[TRT]       binding 1
                -- index   1
                -- name    'NMS'
                -- type    FP32
                -- in/out  OUTPUT
                -- # dims  3
                -- dim #0  1 (SPATIAL)
                -- dim #1  100 (SPATIAL)
                -- dim #2  7 (SPATIAL)
[TRT]       binding 2
                -- index   2
                -- name    'NMS_1'
                -- type    FP32
                -- in/out  OUTPUT
                -- # dims  3
                -- dim #0  1 (SPATIAL)
                -- dim #1  1 (SPATIAL)
                -- dim #2  1 (SPATIAL)
[TRT]    
[TRT]    binding to input 0 Input  binding index:  0
[TRT]    binding to input 0 Input  dims (b=1 c=3 h=300 w=300) size=1080000
[TRT]    binding to output 0 NMS  binding index:  1
[TRT]    binding to output 0 NMS  dims (b=1 c=1 h=100 w=7) size=2800
[TRT]    binding to output 1 NMS_1  binding index:  2
[TRT]    binding to output 1 NMS_1  dims (b=1 c=1 h=1 w=1) size=4
[TRT]    
[TRT]    device GPU, /usr/local/bin/networks/SSD-Mobilenet-v2/ssd_mobilenet_v2_coco.uff initialized.
[TRT]    W = 7  H = 100  C = 1
[TRT]    detectNet -- maximum bounding boxes:  100
[TRT]    detectNet -- loaded 91 class info entries
[TRT]    detectNet -- number of object classes:  91
Using Instance 0
[image] loaded '/tmp/filehLSUCh'  (640x352, 3 channels)
Completed in 940mSeconds
Using Instance 0
[image] loaded '/tmp/fileZRXFZ7'  (640x352, 3 channels)
Completed in 180mSeconds
Using Instance 0
[image] loaded '/tmp/fileXKtgrc'  (640x352, 3 channels)
Completed in 148mSeconds
Using Instance 0
[image] loaded '/tmp/filewdOg3w'  (640x352, 3 channels)
Completed in 277mSeconds
Using Instance 0
[image] loaded '/tmp/fileFW3WLD'  (640x352, 3 channels)
detected obj 0  class #3 (car)  confidence=0.521959
detected obj 1  class #3 (car)  confidence=0.626124
Completed in 146mSeconds
Using Instance 0
[image] loaded '/tmp/fileNbyYBO'  (640x352, 3 channels)
Completed in 149mSeconds
Using Instance 0
[image] loaded '/tmp/filepNBrmj'  (640x352, 3 channels)
Completed in 262mSeconds
Using Instance 0
[image] loaded '/tmp/fileBlao9a'  (640x352, 3 channels)
detected obj 0  class #3 (car)  confidence=0.893681
detected obj 1  class #3 (car)  confidence=0.500488
Completed in 158mSeconds
Using Instance 0
[image] loaded '/tmp/filevuYIQa'  (640x352, 3 channels)
Completed in 122mSeconds
```

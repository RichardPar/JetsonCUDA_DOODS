/*******************************************************
 * Copyright (C) 2010-2011 {Richard} tuxbox.guru@gmail.com
 * 
 * This file is part of cnnserver
 * 
 * cnnserver is beerware - Send me a beer if you find these
 * samples useful 
 *******************************************************/


#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <chrono>
#include <cstdint>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/common.h>
#include <pistache/endpoint.h>

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


// include imageNet header for image recognition
#include <jetson-inference/imageNet.h>
// include loadImage header for loading images
#include <jetson-utils/loadImage.h>
#include "detectNet.h"
#include "base64.hpp"


#define VERSION "0.0Beta"
#define INSTANCES 1


using namespace std;
using namespace Pistache;
using namespace rapidjson;

volatile char busy[INSTANCES];
detectNet *net[INSTANCES];
std::mutex mtx;

inline uint64_t CurrentTime_milliseconds() 
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
              (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

int lockInstance()
{
  int a;
  mtx.lock();

  while (1)
  {
  for (a=0;a<INSTANCES;a++)
   {
     if (busy[a] == 0)
      {
        busy[a]=1;
        cout << "Using Instance " << a << endl;
        mtx.unlock();
        return a;
      }
   }
  usleep(10000);
 }
}

void unlockInstance(int inst)
{
   busy[inst] = 0;
}

struct HelloHandler : public Http::Handler {
  HTTP_PROTOTYPE(HelloHandler)
  void onRequest(const Http::Request& req, Http::ResponseWriter writer) override{
  int ni = lockInstance();

  if (req.resource() == "/version") {
            if (req.method() == Http::Method::Get) {
             string v(VERSION);
             string s = "{\"version\",\"" + v + "\"}";
             writer.send(Http::Code::Ok, s);
             cout << s << endl;
            }
  }

  if (req.resource() == "/detectors") {
      if (req.method() == Http::Method::Get) {
     
         StringBuffer s;
         Writer<StringBuffer> jsonwriter(s);
         jsonwriter.SetMaxDecimalPlaces(7);

         jsonwriter.StartObject();               // Between StartObject()/EndObject(), 
         jsonwriter.Key("detectors");                // output a key,
         jsonwriter.StartArray();
         jsonwriter.StartObject();

         jsonwriter.Key("name");
         jsonwriter.String("default");
         jsonwriter.Key("type");
         jsonwriter.String("tensorflow");
         jsonwriter.Key("model");
         jsonwriter.String(basename(net[ni]->GetModelPath()));

         jsonwriter.Key("labels");
         jsonwriter.StartArray();

         int classCount = net[ni]->GetNumClasses();
         int loop;
         for (loop=0;loop< classCount;loop++)
           jsonwriter.String(net[ni]->GetClassDesc(loop));
         jsonwriter.EndArray();

         jsonwriter.Key("width");
         jsonwriter.Int(net[ni]->GetInputWidth());
         jsonwriter.Key("height");
         jsonwriter.Int(net[ni]->GetInputHeight());

         jsonwriter.Key("channels");
         jsonwriter.Int(3);

         jsonwriter.EndObject();

         jsonwriter.EndArray();
         jsonwriter.EndObject();

         writer.send(Http::Code::Ok,s.GetString());
         cout << s.GetString() << endl;
        }
     } // end of DETECTORS

  if (req.resource() == "/detect") {
      if (req.method() == Http::Method::Post) {
         
          //mtx.lock();
          uint64_t start1 = CurrentTime_milliseconds();
          Document document;
	  StringBuffer s;
          Writer<StringBuffer> jsonwriter(s);
 
          
          string body = req.body();
          uint64_t end = CurrentTime_milliseconds();
          //cout << "File Uploaded took " << (end - start1) << "mSec" << endl;


          document.Parse(body.c_str());
          auto sub = document["detect"].GetObject();

          uint64_t basedec = CurrentTime_milliseconds();
          std::vector<BYTE> decodedData = base64_decode(document["data"].GetString());
          end = CurrentTime_milliseconds();
          //cout << "Base64Decode took " << (end - basedec) << "mSec" << endl;

          char name[30];
          tmpnam(name);
          //cout << "File used is " << name << endl;
          FILE* file = fopen(name, "wb");
          fwrite(&decodedData[0], 1, decodedData.size(), file);
          fclose(file);
          decodedData.clear();

          //ni = lockInstance();
          uchar3* imgPtr = NULL;   // shared CPU/GPU pointer to image
          int imgWidth   = 0;      // width of the image (in pixels)
          int imgHeight  = 0;      // height of the image (in pixels)

          // load the image from disk as uchar3 RGB (24 bits per pixel)
          if( !loadImage(name, &imgPtr, &imgWidth, &imgHeight) )
           {
             printf("failed to load image\n");
            // CLEAN UP THINGS!!!!!   
            writer.send(Http::Code::Ok,"ERROR\r\n");
            unlockInstance(ni);
            remove(name);
            return;
           }
         remove(name);

	 detectNet::Detection* detections = NULL;
         const int numDetections = net[ni]->Detect(imgPtr, imgWidth, imgHeight, &detections, 0);

        jsonwriter.SetMaxDecimalPlaces(7);
        jsonwriter.StartObject();               // Between StartObject()/EndObject(), 
        jsonwriter.Key("detections");                // output a key,
        jsonwriter.StartArray();
        if( numDetections > 0 )
         {
             for( int n=0; n < numDetections; n++ )
                {
                    int addNode=0;

                    for (Value::ConstMemberIterator itr = sub.MemberBegin(); itr != sub.MemberEnd(); ++itr)
                     {
                          string v = itr->name.GetString();
                          string y(net[ni]->GetClassDesc(detections[n].ClassID));
                          if ((v.compare(y)==0) || (v.compare("*")==0))
                            {
                              addNode=1;
                            }
                     }
                    if (addNode == 0)
                        continue; // go back to the start!

                    jsonwriter.StartObject();
                    jsonwriter.Key("top");
                    jsonwriter.Double(detections[n].Top / imgHeight);
                    jsonwriter.Key("left");
                    jsonwriter.Double(detections[n].Left / imgWidth);
                    jsonwriter.Key("bottom");
                    jsonwriter.Double(detections[n].Bottom / imgHeight);
                    jsonwriter.Key("right");
                    jsonwriter.Double(detections[n].Right / imgWidth);
                    jsonwriter.Key("label");
                    jsonwriter.String(net[ni]->GetClassDesc(detections[n].ClassID));
                    jsonwriter.Key("confidence");
                    jsonwriter.Double(detections[n].Confidence*100);
                    printf("detected obj %i  class #%u (%s)  confidence=%f\n", n, detections[n].ClassID, net[ni]->GetClassDesc(detections[n].ClassID), detections[n].Confidence);
            //        printf("bounding box %i  (%f, %f)  (%f, %f)  w=%f  h=%f\n", n, detections[n].Left, detections[n].Top, detections[n].Right, detections[n].Bottom, detections[n].Width(), detections[n].Height());
                    jsonwriter.EndObject();
                 }
         }
        jsonwriter.EndArray();
        jsonwriter.EndObject();
	cudaFreeHost(imgPtr);
        //unlockInstance(ni);
	writer.send(Http::Code::Ok,s.GetString());
        
        end = CurrentTime_milliseconds();
        cout << "Completed in " << (end - start1) << "mSeconds" << endl;
        //mtx.unlock();
      }
   } // End of DETECT
    unlockInstance(ni);

  }
};

int main(int argc, char **argv) {
  commandLine cmdLine(argc, argv);

  int a;
  for (a=0;a<INSTANCES;a++)
   {
     net[a] = detectNet::Create(cmdLine);
     if( !net[a] )
        {
            cout << "detectnet:  failed to load detectNet model\n" << endl;
            return 0;
        }
   }

  Port port(8080);
  Address addr(Ipv4::any(), port);
  auto server = std::make_shared<Http::Endpoint>(addr);
  auto opts = Http::Endpoint::options()
        .maxRequestSize(5*1024*1024)
        .threads(10);

 server->init(opts);
 server->setHandler(Http::make_handler<HelloHandler>());
 server->serve();


//  Http::listenAndServe<HelloHandler>(Pistache::Address("*:9080"));


}

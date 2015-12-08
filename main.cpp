// g++ main2.cpp -ljsoncpp -lmicrohttpd -ljsonrpccpp-common -ljsonrpccpp-server -o sampleserver


// jsonrpcstub test.json --cpp-client=XbmcRemoteClient --cpp-client-file=xbmcremote.h

// sudo apt-get install libjsoncpp-dev 
// sudo apt-get install libjsonrpccpp-dev libjsonrpccpp-tools

// g++ main2.cpp -std=c++11 -ljsoncpp -ljsonrpccpp-common -ljsonrpccpp-client -o sampleclient

#include "xbmcremote.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef WIN32
    #include <termios.h>
#else
    #include <conio.h>
#endif
#include <unistd.h>
#include <time.h>

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>

using namespace jsonrpc;
using namespace cv;
using namespace std;
using namespace zbar;


#include <iostream>

#define LOG(_msg) std::cout << _msg << std::endl;


bool play(XbmcRemoteClient &_cli, int _albumid)
{
    try
    {
        _cli.Playlist_Clear(0);
        Json::Value v;
        v["albumid"]= _albumid;
        _cli.Playlist_Add(0,v);

        Json::Value v1;
        v1["playlistid"]= 0;
        v1["position"]= 0;

        _cli.Player_Open(v1);
    } 
    catch(JsonRpcException& e) {
        cerr << e.what() << endl;
    }
    return true;
}


void videoloop(VideoCapture &_capture, XbmcRemoteClient& _cli)
{
     ImageScanner scanner;
   scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

   double dWidth = _capture.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
   double dHeight = _capture.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

   cout << "Frame size : " << dWidth << " x " << dHeight << endl;

   namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

   int lastAlbum = -1;
   while (1)
   {
      Mat frame;
      bool bSuccess = _capture.read(frame); // read a new frame from video

      if (!bSuccess) //if not success, break loop
      {
         cout << "Cannot read a frame from video stream" << endl;
         break;
      }

      Mat grey;
      cvtColor(frame, grey, CV_BGR2GRAY);

      int width = frame.cols;
      int height = frame.rows;
      uchar *raw = (uchar *)grey.data;

      // wrap image data
      Image image(width, height, "Y800", raw, width * height);

      // scan the image for barcodes
      /*int n = */scanner.scan(image);

      // extract results
      for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
      {
         vector<Point> vp;

         int album = std::atoi(symbol->get_data().c_str());
         if(album != lastAlbum)
         {
            // do something useful with results
            cout << "decoded " << symbol->get_type_name() << " symbol \"" << symbol->get_data() << "\"" << endl;
            if(!play(_cli, album))
            {
                LOG("Could not play album");
            }
            lastAlbum = album;
         }

        int n = symbol->get_location_size();
        for(int i=0;i<n;i++)
        {
           vp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
        }

//         RotatedRect r = minAreaRect(vp);
//         Point2f pts[4];
//         r.points(pts);
// 
//         for(int i=0;i<4;i++){
//           line(frame, pts[i], pts[(i+1)%4], Scalar(255,0,0), 3);
//         }
// 
//         cout<<"Angle: "<<r.angle<<endl;
     }

     imshow("MyVideo", frame); //show the frame in "MyVideo" window

     if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
     {
         cout << "esc key is pressed by user" << endl;
         break;
     }
   }
}

int main(int argc, char **argv)
{
    std::string ip;
    int port = 80;
    if(argc < 2)
    {
        LOG("Error not enough argv");
        exit(-1);
    }
    if(argc == 3)
    {
        port = std::atoi(argv[2]);
    }
    ip = argv[1];
    
    try {
        std::string url = "http://"+ip+":"+std::to_string(port)+"/jsonrpc";
        LOG(url);
        HttpClient httpclient(url.c_str());
        XbmcRemoteClient cli(httpclient);
        
        VideoCapture cap(0);
        if (!cap.isOpened()) // if not success, exit program
        {
            cout << "Cannot open the video cam" << endl;
            return -1;
        }
        videoloop(cap, cli);
        
    } 
    catch(JsonRpcException& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;

}
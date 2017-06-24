import gohai.glvideo.*;
String loopback_frames="";
String loopback_fps="";
String loopback_time="";

String recording_frames="";
String recording_fps="";
String recording_time="";
String recording_size="";
boolean is_recording=false;
GLCapture video;
//void init_camera()
//{
//  String[] devices = GLCapture.list();
//  println("Devices:");
//  printArray(devices);
//  if (0 < devices.length) 
//  {
//    String[] configs = GLCapture.configs(devices[0]);
//    println("Configs:");
//    printArray(configs);
//  }
//  else
//  {
//    println("No Camera Device Found......");
//  }
//video = new GLCapture(this, devices[0], 800, 600, 20);
//}

void init_camera()
{
  int retry_counter=5;
  String[] devices;/* = GLCapture.list();*/
  for(int i=0;i<retry_counter;i++)
  {
    devices=GLCapture.list();
    if(devices.length>0)
    {
      println("Devices:");
      printArray(devices);
      String[] configs = GLCapture.configs(devices[0]);
      println("Configs:");
      printArray(configs);
      video = new GLCapture(this, devices[0]);
//      video = new GLCapture(this, devices[0], 800, 600, 20);
      break;
    }
    else
    {
      println("No Camera Device Found......",i+1);
    }
  }
}


void video_recording_profile()
{
video.start();
draw_controls_video_recording();
}

void monitor_video_recording_profile()
{
  if (video.available())
  {
    video.read();
  }
draw_controls_video_recording();
int cam_window_width=640;
int cam_window_height=480;
image(video, (width/2)-(cam_window_width/2),((height-vertical_padding-control_height-5)/2)-(cam_window_height/2), cam_window_width, cam_window_height);

    text("LOOPBACK DURATION  : "+loopback_time,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("LOOPBACK DURATION   : "+loopback_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+15);
    text("LOOPBACK FPS             : "+loopback_fps,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("LOOPBACK DURATION   : "+loopback_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+15+15);
    text("LOOPBACK FRAMES      : "+loopback_frames,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("LOOPBACK DURATION   : "+loopback_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+15+15+15);

if(is_recording==true)
  {
    text("RECORDING DURATION: "+recording_time,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("RECORDING DURATION : "+recording_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+60+15);
    text("RECORDING FPS           : "+recording_fps,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("RECORDING DURATION : "+recording_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+60+15+15);
    text("RECORDED FRAMES     : "+recording_frames,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("RECORDING DURATION : "+recording_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+60+15+15+15);
    text("VIDEO FILE SIZE           : "+recording_size,((width/2)-(cam_window_width/2)+cam_window_width)-textWidth("RECORDING DURATION : "+recording_time)-5,(((height-vertical_padding-control_height-5)/2)-(cam_window_height/2))+60+15+15+15+15);
  }
}

void draw_controls_video_recording()
{
control_width=150;
control_height=80;

int total_horizontal_controls=3;
int spacing = (width-((control_width*total_horizontal_controls)/*+(padding*2)*/))/(total_horizontal_controls+1);

image(img_background,0,0,width,height);
//image(img_home,home_x,home_y,home_width,home_height);
image(img_button,spacing,height-vertical_padding-control_height,control_width,control_height);
start_recording_button_x=spacing;
start_recording_button_y=height-vertical_padding-control_height;
text("Start Rec",start_recording_button_x+50,start_recording_button_y+control_height/2+5);

image(img_button,spacing+(control_width*1)+(spacing*1),height-vertical_padding-control_height,control_width,control_height);
stop_recording_button_x=spacing+(control_width*1)+(spacing*1);
stop_recording_button_y=height-vertical_padding-control_height;
text("Stop Rec",stop_recording_button_x+50,stop_recording_button_y+control_height/2+5);

int home_width=80;
image(img_home,spacing+(control_width*2)+(spacing*2),height-vertical_padding-control_height,home_width,control_height);
home_x=spacing+(control_width*2)+(spacing*2);
home_y=height-vertical_padding-control_height;
}

boolean start_video_recording()
{
    String s="1234567!@#$%^&";
  String s1="1234567!@#$%^&";
  int count=0;
  int retry_count=0;
  Process p;
  println("/dev/video2 device available.......Starting recording!!!");
  println("sudo /home/pi/harvind/FFmpeg/ffmpeg -s 800x600 -i /dev/video2 -f alsa -ac 1 -i hw:1,0 -acodec aac -ar 44100 -async 1 -ab 96k -c:v h264_omx -b:v 10000k "+"/tmp/"+year()+"-"+month()+"-"+day()+"-"+hour()+"-"+minute()+"-"+second()+".avi");
while(retry_count++ < 10)//retrys too detect camera in case it is not initialised. 
  {
    println("Attempt.....",retry_count);
    count=0;
    s="1234567!@#$%^&";
    s1="1234567!@#$%^&";
        try
        {
//          p=execute_command("sudo /home/pi/harvind/FFmpeg/ffmpeg -s 800x600 -i /dev/video2 -f alsa -ac 1 -i hw:1,0 -acodec aac -ar 44100 -async 1 -ab 96k -c:v h264_omx -b:v 10000k /tmp/$(date +\"%d-%m-%y-%H-%M-%S\").avi");
//          p=execute_command("sudo /home/pi/harvind/FFmpeg/ffmpeg -s 800x600 -i /dev/video2 -f alsa -ac 1 -i hw:1,0 -acodec aac -ar 44100 -async 1 -ab 96k -c:v h264_omx -b:v 10000k "+"/tmp/"+year()+"-"+month()+"-"+day()+"-"+hour()+"-"+minute()+"-"+second()+".avi");
          p=execute_command("sudo /home/pi/harvind/FFmpeg/ffmpeg -y -s 800x600 -i /dev/video2 -c:v h264_omx -b:v 10000k "+"/tmp/"+year()+"-"+month()+"-"+day()+"-"+hour()+"-"+minute()+"-"+second()+".avi");
          ffmpeg_r=p;
          BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));  
          while(count++ <30)
          {
            s = stdError.readLine();
            if(s!=null)
            {
              s1=s;
              println(s1);
            }
            else break;      
          }
        }
        catch(IOException e)
        {
                  System.out.println("exception happened - start_ffmpeg_video_copy() ");
                  e.printStackTrace();
                  System.exit(-1);   
        }
      println("s1 before compare....",s1);
      if(s1.equals("/dev/video0: Device or resource busy")==true || s1.equals("/dev/video0: No such file or directory")==true)
      {
        delay(2000);
        println("loop again...");
      }
      else break;
  }
//if(s1.equals("/dev/video0: Device or resource busy")==true)
 if(retry_count==10)
 {
   println("Failed to run ffmpeg :(");
   return false;
 }
   println("ffmpeg run Successfull :)");
   return true;
}
void stop_video_recording()
{
  ffmpeg_r.destroy();
}
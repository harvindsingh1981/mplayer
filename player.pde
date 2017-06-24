/*
Audio Video Player along with Photo viewer
*/
//PGraphics visulizer; 
String current_profile="";
int profile_switch=0;
String renderer="";
WebsocketServer ws;
Process omx_p;
Process ffmpeg_c;
Process ffmpeg_r;
void setup()
{
size(800,600,P3D);
//fullScreen(P2D);
renderer="p3d";
slider_width=width-(horizontal_padding*2);//initialized Global variable because width is iniatialized after calling setup()
//create_countdown_timers();
println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Audio Library....Start");
minim = new Minim(this);//load library for Audio
println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Audio Library....End");

println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Images....Start");
load_images();
println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Images....End");

println("["+minute()+":"+second()+":"+millis()+"]"+"Loading v4l2loopback Driver....Start");
insert_v4l2loopback_driver();//insert v4l2loopback driver to create vodeo loopback nodes /dev/video1 and /dev/video2
println("["+minute()+":"+second()+":"+millis()+"]"+"Loading v4l2loopback Driver....End");

println("["+minute()+":"+second()+":"+millis()+"]"+"Open WebSocket....Start");
ws=new WebsocketServer(this,8025,"/omx");
println("["+minute()+":"+second()+":"+millis()+"]"+"Open WebSocket....End");

println("["+minute()+":"+second()+":"+millis()+"]"+"ffmpeg video copy....Start");
start_ffmpeg_video_copy();
thread("ffmpeg_copy_monitoring_thread");
println("["+minute()+":"+second()+":"+millis()+"]"+"ffmpeg video copy....End");

println("["+minute()+":"+second()+":"+millis()+"]"+"Run omx.js....Start");
omx_p=execute_command("node /home/pi/Desktop/omx.js"+" "+str(get_screen_x())+" "+str(get_screen_y())+" "+str(get_screen_x()+width)+" "+str(get_screen_y()+(height-vertical_padding-control_height-50-20)));//run omx.js javascript for controlling omxplayer over dbus
println(omx_p);
println("["+minute()+":"+second()+":"+millis()+"]"+"Run omx.js....End");
video_x=get_screen_x();
video_y=get_screen_y();
video_x_prev=video_x;
video_y_prev=video_y;

println("["+minute()+":"+second()+":"+millis()+"]"+"Init Camera....Start");
init_camera();
println("["+minute()+":"+second()+":"+millis()+"]"+"Init Camera....End");
}

void draw()
{
  switch(current_profile)
  {
    case "":
    current_profile="HOME";
    home_profile();
    break;
    
    case "HOME":
      if(profile_switch==1)
        {
          home_profile();
          profile_switch=0;
        }
       monitor_home_profile();   
    break;
    
    case "AUDIO":
      if(profile_switch==1)
        {
          audio_profile();
          profile_switch=0;
        }
      draw_controls_audio();
      monitor_audio_profile();
    break;
    
    case "VIDEO":
      if(profile_switch==1)
        {
          video_profile();
          profile_switch=0;
        }
       monitor_video_profile();   
    break;

    case "VIDEO_RECORDING":
      if(profile_switch==1)
        {
          video_recording_profile();
          profile_switch=0;
        }
       monitor_video_recording_profile();   
    break;

    case "IMAGE":
    break;
  }
}
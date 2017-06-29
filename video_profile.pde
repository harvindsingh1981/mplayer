import processing.awt.PSurfaceAWT;
import processing.awt.PSurfaceAWT.SmoothCanvas;//for getting output window cordinates in normal renderer mode
import com.jogamp.newt.opengl.GLWindow;//for getting output window cordinates in P3D and P2D renderer mode
import websockets.*;//for opening websocket server to communicate with omx.js (acting as client) for sending commands and getting status from omxplayer.

String video_state="PAUSE";
int video_x=0;
int video_y=0;
int video_x_prev=0;
int video_y_prev=0;
boolean is_video_playing=false;
boolean is_video_fullscreen=false;
String video_file_path="/tmp/2017-6-26-12-47-47.avi";//"/home/pi/Downloads/Sintel_DivXPlus_6500kbps.mkv";
String video_duration="00000000";
String video_position="00000000";
String video_pause_status="";

int get_screen_x()//get output screen x cordinates
{
  int x;
  GLWindow glw = (GLWindow)surface.getNative();
    if(renderer=="p3d" || renderer=="p2d")x=glw.getX();
    else x=((SmoothCanvas)((PSurfaceAWT)surface).getNative()).getFrame().getX();
//    println("X=",x);
return x;
}

int get_screen_y()//get output screen y cordinates
{
  int y;
  GLWindow glw = (GLWindow)surface.getNative();
    if(renderer=="p3d" || renderer=="p2d")y=glw.getY();
    else y=((SmoothCanvas)((PSurfaceAWT)surface).getNative()).getFrame().getY();
//    println("Y=",y);
return y;
}

void draw_controls_video()
{
int total_horizontal_controls=4;
int spacing = (width-((control_width*total_horizontal_controls)/*+(padding*2)*/))/(total_horizontal_controls+1);

control_width=80;
control_height=80;

image(img_background,0,0,width,height);
//image(img_home,home_x,home_y,home_width,home_height);
image(img_play_pause,spacing,height-vertical_padding-control_height,control_width,control_height);
play_pause_x=spacing;
play_pause_y=height-vertical_padding-control_height;
image(img_stop,spacing+(control_width*1)+(spacing*1),height-vertical_padding-control_height,control_width,control_height);
stop_x=spacing+(control_width*1)+(spacing*1);
stop_y=height-vertical_padding-control_height;
image(img_volume,spacing+(control_width*2)+(spacing*2),height-vertical_padding-control_height,control_width,control_height);
volume_x=spacing+(control_width*2)+(spacing*2);
volume_y=height-vertical_padding-control_height;
image(img_home,spacing+(control_width*3)+(spacing*3),height-vertical_padding-control_height,control_width,control_height);
home_x=spacing+(control_width*3)+(spacing*3);
home_y=height-vertical_padding-control_height;
fill(0,255,0);
rect(horizontal_padding,height-vertical_padding-control_height-50,slider_width,slider_height);//slider
noFill();
slider_x=horizontal_padding;
slider_y=height-vertical_padding-control_height-50;
ball_x=horizontal_padding-(ball_width/2);
ball_y=height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2);
}
void update_time_info()
{
float dur_s;
float pos_s;
double total_duration=Double.parseDouble(video_position);
double duration_ms=total_duration/1000;
double duration_s=duration_ms/1000;
pos_s=(float)duration_s;
int hours=(int)(duration_s/3600);
int min=(int)((duration_s%3600)/60);
int sec=(int)((duration_s%3600)%60);
text(nf(hours,2) + ":" + nf(min,2) + ":"+nf(sec,2),horizontal_padding,height-vertical_padding-control_height-50+slider_height+20);
total_duration=Double.parseDouble(video_duration);
duration_ms=total_duration/1000;
duration_s=duration_ms/1000;
dur_s=(float)duration_s;
hours=(int)(duration_s/3600);
min=(int)((duration_s%3600)/60);
sec=(int)((duration_s%3600)%60);
text("/"+nf(hours,2) + ":" + nf(min,2) + ":"+nf(sec,2),horizontal_padding+textWidth(nf(hours,2) + ":" + nf(min,2) + ":"+nf(sec,2)),height-vertical_padding-control_height-50+slider_height+20);

float ball_current_pos=ball_x;
ball_current_pos=map(pos_s,0,dur_s,0+horizontal_padding-(ball_width/2),width-horizontal_padding-(ball_width/2));
image(img_ball,ball_current_pos,height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2),ball_width,ball_height);//slider ball
}

void video_profile()
{
draw_controls_video();
if(is_omx_running)
{
  send_message_to_omx("unhidevideo","");
  if(video_state=="PLAY")
  {
    send_message_to_omx("pause","");//pause toggles play and pause
  }
}
else 
  {
     send_message_to_omx("start","");
     video_state="PLAY";
  }
}

void monitor_video_profile()
{
draw_controls_video();
monitor_video_pos();
update_time_info();
}

void monitor_video_pos()
{
  video_x=get_screen_x();
  video_y=get_screen_y();
if(video_x!=video_x_prev || video_y!=video_y_prev)
  {
     send_message_to_omx("setvideopos",str(video_x)+" "+str(video_y)+" "+str(video_x+width)+" "+str(video_y+(height-vertical_padding-control_height-50-20)));
     video_x_prev=video_x;
     video_y_prev=video_y;
  }
}
boolean is_omx_running=false;

boolean check_if_omx_running()
{
  Process p=null;
  String s;
  String[] parsed_data;
  try
  {
            p = Runtime.getRuntime().exec("/home/pi/Desktop/dbuscontrol.sh getsource");
            System.out.println(p);
            BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            while ((s = stdError.readLine()) != null) 
            {
              parsed_data=split(s,":");
              if(parsed_data[0].equals("Error org.freedesktop.DBus.Error.ServiceUnknown"))return false;
            }
  }
  catch(IOException e)
  {
            System.out.println("[check_if_omx_running]exception happened - here's what I know: ");
            e.printStackTrace();
            return false;
  }
return true;  
}

void send_message_to_omx(String command,String param)
{
  //Available Commands
  //status|pause|stop|seek|volumeup|volumedown|setposition [position in microseconds]|hidevideo|unhidevideo|togglesubtitles|hidesubtitles|showsubtitles|setvideopos [x1 y1 x2 y2]|setvideocroppos [x1 y1 x2 y2]
  //|setaspectmode [letterbox,fill,stretch,default]|setalpha [alpha (0..255)]|getsource
  String final_command="";
  String s;
  if(param.equals("")==true)final_command="/home/pi/Desktop/dbuscontrol.sh"+" "+command;//+" "+param;
  else final_command="/home/pi/Desktop/dbuscontrol.sh"+" "+command+" "+param;
    try
    {
      if(command.equals("start")==true)
      {
        thread("start_omx");
        return;
      }
      Process p = Runtime.getRuntime().exec(final_command);
      println(p);
      BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getInputStream()));
      while ((s = stdError.readLine()) != null) 
      {
        println(s);
      }
    }
  catch(IOException e)
    {
       System.out.println("[send_command_to_omx]exception happened - here's what I know: ");
       e.printStackTrace();
    }    
}

//void webSocketServerEvent(String msg)//websocket event handler
//{
// println(msg);
//}

//void send_message_to_omx(String command,String msg)
//{
//  ws.sendMessage(command);
//  ws.sendMessage(msg);
//  if(command.equals("START")==true)is_video_playing=true;
//}
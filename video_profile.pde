import processing.awt.PSurfaceAWT;
import processing.awt.PSurfaceAWT.SmoothCanvas;//for getting output window cordinates in normal renderer mode
import com.jogamp.newt.opengl.GLWindow;//for getting output window cordinates in P3D and P2D renderer mode
import websockets.*;//for opening websocket server to communicate with omx.js (acting as client) for sending commands and getting status from omxplayer.

String video_state="PAUSE";
int video_x=0;
int video_y=0;
int video_x_prev=0;
int video_y_prev=0;

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

void video_profile()
{
draw_controls_video();
send_message_to_omx("START","/home/pi/Downloads/Sintel_DivXPlus_6500kbps.mkv");
}

void monitor_video_profile()
{
draw_controls_video();
monitor_video_pos();
//send_message_to_omx("GET_POS"," ");
}

void webSocketServerEvent(String msg)//websocket event handler
{
 println(msg);
}

void send_message_to_omx(String command,String msg)
{
  ws.sendMessage(command);
  ws.sendMessage(msg);
}

void monitor_video_pos()
{
  video_x=get_screen_x();
  video_y=get_screen_y();
if(video_x!=video_x_prev || video_y!=video_y_prev)
  {
     send_message_to_omx("VIDEO_POS",str(video_x)+" "+str(video_y)+" "+str(video_x+width)+" "+str(video_y+(height-vertical_padding-control_height-50-20)));
     video_x_prev=video_x;
     video_y_prev=video_y;
  }
}
void ffmpeg_copy_monitoring_thread()
{
  String[] recording_params;
  try
  {
      String s;
      println("In ffmpeg copy monitoring thread");
      
      BufferedReader stdError = new BufferedReader(new InputStreamReader(ffmpeg_c.getErrorStream()));
      // read any errors from the attempted command
      System.out.println("Here is the standard error of the command (if any):\n");
      while ((s = stdError.readLine()) != null) 
        {
          recording_params=splitTokens(s,"= ");
          if(recording_params[0].equals("frame")==true)
          {
            for(int i=0;i<recording_params.length;i++)
            {
              if(recording_params[i].equals("frame"))loopback_frames=recording_params[i+1];
              if(recording_params[i].equals("fps"))loopback_fps=recording_params[i+1];
              if(recording_params[i].equals("time"))loopback_time=recording_params[i+1];
            }
          }

          System.out.println("[ffmpeg_copy_monitoring_thread]"+s);
        }
  } 
  catch(IOException e)
  {
     System.out.println("[ffmpeg_copy_monitoring_thread]exception happened - ffmpeg_copy_monitoring_thread");
     e.printStackTrace();
//     System.exit(-1);   
  }  
}

void ffmpeg_recording_monitoring_thread()
{
  println("In ffmpeg recording monitoring thread");
  try
  {
      String s;
      String[] recording_params;
      println("In ffmpeg copy monitoring thread");
      
      BufferedReader stdError = new BufferedReader(new InputStreamReader(ffmpeg_r.getErrorStream()));
      // read any errors from the attempted command
      System.out.println("Here is the standard error of the command (if any):\n");
      while ((s = stdError.readLine()) != null) 
        {
          recording_params=splitTokens(s,"= ");
          if(recording_params[0].equals("frame")==true)
          {
            for(int i=0;i<recording_params.length;i++)
            {
              if(recording_params[i].equals("frame"))recording_frames=recording_params[i+1];
              if(recording_params[i].equals("fps"))recording_fps=recording_params[i+1];
              if(recording_params[i].equals("time"))recording_time=recording_params[i+1];
              if(recording_params[i].equals("size"))recording_size=recording_params[i+1];
            }
          }
           System.out.println("[ffmpeg_recording_monitoring_thread]"+s);
        }
  } 
  catch(IOException e)
  {
     System.out.println("[ffmpeg_recording_monitoring_thread]exception happened - ffmpeg_recording_monitoring_thread ");
     e.printStackTrace();
//     System.exit(-1);   
  }  
}

void start_omx()
{
  Process p;
  println("In start_omx thread");
  try
  {
      String s;
      println("omxplayer"+" "+video_file_path+" "+"--no-keys"+" "+"--no-osd"+" "+"--win "+" "+str(get_screen_x())+","+str(get_screen_y())+","+str(get_screen_x()+width)+","+str(get_screen_y()+(height-vertical_padding-control_height-50-20)));
      p = Runtime.getRuntime().exec("omxplayer"+" "+video_file_path+" "+"--no-keys"+" "+"--no-osd"+" "+"--win "+" "+str(get_screen_x())+","+str(get_screen_y())+","+str(get_screen_x()+width)+","+str(get_screen_y()+(height-vertical_padding-control_height-50-20)));
      delay(700);
      is_video_playing=true;
      is_omx_running=true;
      video_state="PLAY";
      thread("omx_player_status_thread");
      send_message_to_omx("setaspectmode","fill");//[letterbox,fill,stretch,default]
      BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
      while ((s = stdError.readLine()) != null) 
        {
           System.out.println("[start_omx_thread]"+s);
        }
      is_video_playing=false;
      is_omx_running=false;
      video_state="PAUSE";
  } 
  catch(IOException e)
  {
     System.out.println("[start_omx_thread]exception happened - start_omx_thread ");
     e.printStackTrace();
//     System.exit(-1);   
      is_video_playing=false;
      is_omx_running=false;
      video_state="PAUSE";
  }    
}

void omx_player_status_thread()
{
 String s;
 String[] params;
    while(is_omx_running==true)
    {
    delay(1000);
      try
      {
       if(current_profile=="VIDEO")
        {
          Process p = Runtime.getRuntime().exec("/home/pi/Desktop/dbuscontrol.sh status");
          println(p);
          BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getInputStream()));
          while ((s = stdError.readLine()) != null) 
          {
            params=splitTokens(s,": ");
            if(params[0].equals("Duration")==true || params[0].equals("Position")==true || params[0].equals("Paused")==true)
            {
              for(int i=0;i<params.length;i++)
              {
//                println(params[i]);
                if(params[i].equals("Duration"))video_duration=params[i+1];
                if(params[i].equals("Position"))video_position=params[i+1];
                if(params[i].equals("Paused"))video_pause_status=params[i+1];
              }
            }
            println("["+millis()+"]"+"video_duration :"+video_duration);
            println("["+millis()+"]"+"video_position :"+video_position);
            println("["+millis()+"]"+"video_pause_status :"+video_pause_status);
          }
        }
      }
      catch(IOException e)
      {
         System.out.println("[send_command_to_omx]exception happened - here's what I know: ");
         e.printStackTrace();
      }
    }
}


//void load_minim_thread()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Audio Library....Start");
//  minim = new Minim(this);//load library for Audio
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Audio Library....End");
////  is_minim_loaded=true;
//}

//void load_image_thread()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Images....Start");
//  load_images();
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Loading Images....End");
////  is_image_loaded=true;
//}
//void load_v4l2loopback_driver()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Loading v4l2loopback Driver....Start");
//  insert_v4l2loopback_driver();//insert v4l2loopback driver to create vodeo loopback nodes /dev/video1 and /dev/video2
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Loading v4l2loopback Driver....End");
////  is_v4l2loopback_driver_loaded=true;
//}

//void load_websocket_thread()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Open WebSocket....Start");
//  ws=new WebsocketServer(this,8025,"/omx");
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Open WebSocket....End");
////  is_websocket_loaded=true;
//}

//void load_ffmpeg_video_copy()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"ffmpeg video copy....Start");
//  start_ffmpeg_video_copy();
//  thread("ffmpeg_copy_monitoring_thread");
//  println("["+minute()+":"+second()+":"+millis()+"]"+"ffmpeg video copy....End");
////  is_ffmpeg_video_copy_loaded=true;
//}

//void load_omx()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Run omx.js....Start");
//  omx_p=execute_command("node /home/pi/Desktop/omx.js"+" "+str(get_screen_x())+" "+str(get_screen_y())+" "+str(get_screen_x()+width)+" "+str(get_screen_y()+(height-vertical_padding-control_height-50-20)));//run omx.js javascript for controlling omxplayer over dbus
//  println(omx_p);
//  video_x=get_screen_x();
//  video_y=get_screen_y();
//  video_x_prev=video_x;
//  video_y_prev=video_y;
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Run omx.js....End");
////  is_omx_loaded=true;
//}

//void load_camera()
//{
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Init Camera....Start");
////  while(is_ffmpeg_video_copy_loaded==false)delay(100);
//  init_camera();
//  println("["+minute()+":"+second()+":"+millis()+"]"+"Init Camera....End");
////  is_camera_loaded=true;
//}
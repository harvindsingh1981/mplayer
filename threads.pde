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
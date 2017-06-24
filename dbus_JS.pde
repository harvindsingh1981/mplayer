
//import javax.script.*;
//ScriptEngineManager manager = new ScriptEngineManager();
//ScriptEngine engine = manager.getEngineByName("JavaScript");
//void run_js()
//{
//    try
//    {
//      engine.eval("print('Hello World');");
//    }
//    catch(ScriptException e)
//    {
//      e.printStackTrace();
//    }
//}

import java.io.*;
Process execute_command(String command)
{
  Process p=null;
  try
  {
            p = Runtime.getRuntime().exec(command);
            System.out.println(p);
  }
  catch(IOException e)
  {
            System.out.println("exception happened - here's what I know: ");
            e.printStackTrace();
            System.exit(-1);   
  }
return p;
}

void insert_v4l2loopback_driver()
{
  //check if driver already inserted
  File v1=new File("/dev/video1");
  File v2=new File("/dev/video2");
  if(v1.exists() && v2.exists())
  {
    println("v4l2loopback Driver already inserted !!!!");
  }
  else
  {
    println("Inserting v4l2loopback Driver");
    println("sudo insmod /lib/modules/4.9.31-v7/extra/v4l2loopback.ko devices=2 exclusive_caps=1");
    execute_command("sudo insmod /lib/modules/4.9.31-v7/extra/v4l2loopback.ko devices=2 exclusive_caps=1");
    delay(1000);
    println("Done !!!");
  }
}

boolean start_ffmpeg_video_copy()
{
  String s="1234567!@#$%^&";
  String s1="1234567!@#$%^&";
  int count=0;
  int retry_count=0;
  Process p;
  println("Starting ffmpeg for copying video0 to video1 & video2");
  println("/home/pi/harvind/FFmpeg/ffmpeg -f video4linux2 -s 800x600 -i /dev/video0 -codec copy -f v4l2 /dev/video1 -codec copy -f v4l2 /dev/video2");
while(retry_count++ < 10)//retrys too detect camera in case it is not initialised. 
  {
    println("Attempt.....",retry_count);
    count=0;
    s="1234567!@#$%^&";
    s1="1234567!@#$%^&";
        try
        {
          p=execute_command("sudo /home/pi/harvind/FFmpeg/ffmpeg -f video4linux2 -s 800x600 -i /dev/video0 -codec copy -f v4l2 /dev/video1 -codec copy -f v4l2 /dev/video2");
          ffmpeg_c=p;
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

//void ffmpeg_video_copy_monitor_thread_start(Process p)
//{
//  final BufferedReader rdr = new BufferedReader(new InputStreamReader(p.getErrorStream()));
//      Thread t = new Thread() {
//        public void run() {
//            try {
//                for (String line; (line = rdr.readLine()) != null;) 
//                {
//                    //queue.put(line);
//                    System.out.println(line);
//                }
//            } catch (Exception e) {
//                e.printStackTrace();
//            }
//        }
//    };
//    t.start();
//}

//void execute_command(String command)
//{
////  String s=null;
//  try
//  {
//            // run the Unix "ps -ef" command
//            // using the Runtime exec method:
//            Process p = Runtime.getRuntime().exec(command);
//            System.out.println(p);
            
//            //BufferedReader stdInput = new BufferedReader(new 
//            //     InputStreamReader(p.getInputStream()));

//            //BufferedReader stdError = new BufferedReader(new 
//            //     InputStreamReader(p.getErrorStream()));

//            //// read the output from the command
//            //System.out.println("Here is the standard output of the command:\n");
//            //while ((s = stdInput.readLine()) != null) {
//            //    System.out.println(s);
//            //}
            
//            //// read any errors from the attempted command
//            //System.out.println("Here is the standard error of the command (if any):\n");
//            //while ((s = stdError.readLine()) != null) {
//            //    System.out.println(s);
//            //}
            
//            //System.exit(0);
//  }
//  catch(IOException e)
//  {
//            System.out.println("exception happened - here's what I know: ");
//            e.printStackTrace();
//            System.exit(-1);   
//  }
//}

//void start_omxplayer(String command)
//{
//  try
//  {
//  Process p = Runtime.getRuntime().exec(command);
//  System.out.println(p);
//  }
//  catch(IOException e)
//  {
//    System.out.println("exception happened - here's what I know: ");
//    e.printStackTrace();
//    System.exit(-1);   
//  }
//}

void kill_omxplayer()
{
  try
  {
     Process p = Runtime.getRuntime().exec("pkill omxpalyer.bin");
     System.out.println(p);
  }
  catch(IOException e)
  {
     System.out.println("exception happened - here's what I know: ");
     e.printStackTrace();
     System.exit(-1);   
  }
}

//public String tail( File file ) { //reads last line of file
//    RandomAccessFile fileHandler = null;
//    try {
//        fileHandler = new RandomAccessFile( file, "r" );
//        long fileLength = fileHandler.length() - 1;
//        StringBuilder sb = new StringBuilder();

//        for(long filePointer = fileLength; filePointer != -1; filePointer--){
//            fileHandler.seek( filePointer );
//            int readByte = fileHandler.readByte();

//            if( readByte == 0xA ) {
//                if( filePointer == fileLength ) {
//                    continue;
//                }
//                break;

//            } else if( readByte == 0xD ) {
//                if( filePointer == fileLength - 1 ) {
//                    continue;
//                }
//                break;
//            }

//            sb.append( ( char ) readByte );
//        }

//        String lastLine = sb.reverse().toString();
//        return lastLine;
//    } catch( java.io.FileNotFoundException e ) {
//        e.printStackTrace();
//        return null;
//    } catch( java.io.IOException e ) {
//        e.printStackTrace();
//        return null;
//    } finally {
//        if (fileHandler != null )
//            try {
//                fileHandler.close();
//            } catch (IOException e) {
//                /* ignore */
//            }
//    }
//}

import javax.script.*;
ScriptEngineManager manager = new ScriptEngineManager();
ScriptEngine engine = manager.getEngineByName("JavaScript");
void run_js()
{
    try
    {
      engine.eval("print('Hello World');");
    }
    catch(ScriptException e)
    {
      e.printStackTrace();
    }
}

import java.io.*;
void execute_command()
{
  String s=null;
  try
  {
            // run the Unix "ps -ef" command
            // using the Runtime exec method:
            Process p = Runtime.getRuntime().exec("node /home/pi/Desktop/omx.js"+" "+str(get_screen_x())+" "+str(get_screen_y())+" "+str(get_screen_x()+width)+" "+str(get_screen_y()+(height-vertical_padding-control_height-50-20)));
            System.out.println(p);
            
            //BufferedReader stdInput = new BufferedReader(new 
            //     InputStreamReader(p.getInputStream()));

            //BufferedReader stdError = new BufferedReader(new 
            //     InputStreamReader(p.getErrorStream()));

            //// read the output from the command
            //System.out.println("Here is the standard output of the command:\n");
            //while ((s = stdInput.readLine()) != null) {
            //    System.out.println(s);
            //}
            
            //// read any errors from the attempted command
            //System.out.println("Here is the standard error of the command (if any):\n");
            //while ((s = stdError.readLine()) != null) {
            //    System.out.println(s);
            //}
            
            //System.exit(0);
  }
  catch(IOException e)
  {
            System.out.println("exception happened - here's what I know: ");
            e.printStackTrace();
            System.exit(-1);   
  }
}
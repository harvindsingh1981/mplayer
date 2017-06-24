void exit()
{
    try
  {
            Process p = Runtime.getRuntime().exec("pkill omxplayer.bin");
            System.out.println(p);
  }
  catch(IOException e)
  {
            System.out.println("exception happened - here's what I know: ");
            e.printStackTrace();
            System.exit(-1);   
  }
  
  println("Sketch is closing.........Buyyy");
  System.exit(0);
}
import ddf.minim.*;
import ddf.minim.analysis.*;
AudioMetaData meta;
Minim minim;
AudioPlayer player;
FFT fft;

String title="";

void load_audio_file(String filename)
{
  player = minim.loadFile(filename);
  meta = player.getMetaData();
  title = meta.title();
}

void display_audio_info()
{
  text(title,width/2 - 80,height/2 - 100);  
}
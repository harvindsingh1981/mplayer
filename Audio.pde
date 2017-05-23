import ddf.minim.*;
import ddf.minim.analysis.*;
AudioMetaData meta;
Minim minim;
AudioPlayer player;
FFT fft;
String state="PAUSE";


void load_audio_file(String filename)
{
  player = minim.loadFile(filename);
  fft = new FFT( player.bufferSize(), player.sampleRate());
}

void display_audio_info()
{
  int info_box_x=horizontal_padding;
  int info_box_y=vertical_padding;
  int info_box_width=350;
  int info_box_height=150;
  int info_box_corner_radius=0;
  int text_vertical_spacing=18;
  int y;
  String temp_string="";
  
  meta = player.getMetaData();

  rect(info_box_x,info_box_y,info_box_width,info_box_height,info_box_corner_radius);
  y=info_box_y+20;
  temp_string="Title : " + meta.title();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="Author : " + meta.author();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="Album : " + meta.album();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="lyrics : " + meta.lyrics();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="Track : " + meta.track();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="Genre : " + meta.genre();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="Composer : " + meta.composer();
  text(temp_string,info_box_x+5,y);

  y=y+text_vertical_spacing;
  temp_string="Orchestra : " + meta.orchestra();
  text(temp_string,info_box_x+5,y);
}

//void audio_visulizer()
//{
//  int visulizer_x=100;
//  int visulizer_y=(height/2)-(visulizer.height/2);
//  int rect_level_height=(visulizer.height/4);
//  int rect_level_left_y=(visulizer.height/4)-(visulizer.height/8);
//  int rect_level_right_y=(visulizer.height/4)*3-(visulizer.height/8);
//  int oscilator_left_y=(visulizer.height/4);
//  int oscilator_right_y=(visulizer.height/4)*3;
//  int oscilator_gain=rect_level_height;
  
//  visulizer.beginDraw();
//  visulizer.background(0);
//  visulizer.fill( 0,255,0);
//  visulizer.rect( 0, rect_level_left_y/*25*/, player.left.level()*visulizer.width*2.0, rect_level_height/*50*/ );
//  visulizer.rect( 0, rect_level_right_y/*125*/, player.right.level()*visulizer.width*2.0, rect_level_height/*50*/ );

//  visulizer.stroke( 255 );
//  // draw the waveforms
//  // the values returned by left.get() and right.get() will be between -1 and 1,
//  // so we need to scale them up to see the waveform
//  // note that if the file is MONO, left.get() and right.get() will return the same value
//  for(int i = 0; i < player.bufferSize() - 1; i++)
//  {
//    float x1 = map( i, 0, player.bufferSize(), 0, visulizer.width);
//    float x2 = map( i+1, 0, player.bufferSize(), 0, visulizer.width);
//    visulizer.line( x1, oscilator_left_y/*50*/ + player.left.get(i)*oscilator_gain/*50*/, x2, oscilator_left_y/*50*/ + player.left.get(i+1)*oscilator_gain/*50*/ );
//    visulizer.line( x1, oscilator_right_y/*150*/ + player.right.get(i)*oscilator_gain/*50*/, x2, oscilator_right_y/*150*/ + player.right.get(i+1)*oscilator_gain/*50*/ );
//  } 
////  noStroke();
//visulizer.endDraw();
//image(visulizer,visulizer_x,visulizer_y);
//}

void audio_visulizer()
{
  int visulizer_width=width-(control_width*2)-20-20; //500;
  int visulizer_height=200;
  int visulizer_x=horizontal_padding+control_width;
  int visulizer_y=(height/2)-(visulizer_height/2);
  int rect_level_height=(visulizer_height/4);
  int rect_level_left_y=visulizer_y+(visulizer_height/4)-(visulizer_height/8);
  int rect_level_right_y=visulizer_y+(visulizer_height/4)*3-(visulizer_height/8);
  int oscilator_left_y=visulizer_y+(visulizer_height/4);
  int oscilator_right_y=visulizer_y+(visulizer_height/4)*3;
  int oscilator_gain=rect_level_height;
  noFill();
  rect(visulizer_x,visulizer_y,visulizer_width,visulizer_height);
  fill( 0,255,0);
  rect( visulizer_x, rect_level_left_y/*25*/, player.left.level()*visulizer_width*2.0, rect_level_height/*50*/ );
  rect( visulizer_x, rect_level_right_y/*125*/, player.right.level()*visulizer_width*2.0, rect_level_height/*50*/ );

  stroke( 255 );
  for(int i = 0; i < player.bufferSize() - 1; i++)
  {
    float x1 = map( i, 0, player.bufferSize(),visulizer_x/* 0*/, visulizer_x+visulizer_width);
    float x2 = map( i+1, 0, player.bufferSize(),visulizer_x/* 0*/,visulizer_x+visulizer_width);
    line( x1, oscilator_left_y/*50*/ + player.left.get(i)*oscilator_gain/*50*/, x2, oscilator_left_y/*50*/ + player.left.get(i+1)*oscilator_gain/*50*/ );
    line( x1, oscilator_right_y/*150*/ + player.right.get(i)*oscilator_gain/*50*/, x2, oscilator_right_y/*150*/ + player.right.get(i+1)*oscilator_gain/*50*/ );
  }
switch (state)
  {
    case "PAUSE" :
    text("PAUSED", visulizer_x+(visulizer_width/2)-(textWidth("PAUSED")/2),visulizer_y+(visulizer_height/2));
    break;
    case "STOP" :
    text("STOPPED", visulizer_x+(visulizer_width/2)-(textWidth("PAUSED")/2),visulizer_y+(visulizer_height/2));
    break;
  }

}

void audio_visulizer_bar()
{
  int visulizer_width=width-(control_width*2)-20-20; //500;
  int visulizer_height=200;
  int visulizer_x=horizontal_padding+control_width;
  int visulizer_y=(height/2)-(visulizer_height/2);
  int y=0;
  float bar_height;
  int bar_width;
  int bar_spacing=2;//5;
  int sample_interval=1;//5;
  int gain=1;
fft.forward( player.mix );
int spectrum_size=fft.specSize()/(/*2*/sample_interval);
bar_width=(visulizer_width-(bar_spacing*(spectrum_size+1)))/spectrum_size;
rectMode(CORNERS); //<>//
fill(128,128,128);
  for (int i = 0; i < fft.specSize ()/*/2*/; i=i+sample_interval) 
  {
    if(i<50)gain=2;
    else gain=20;
    bar_height=visulizer_y+visulizer_height - fft.getBand(i)*gain;
    if(bar_height<visulizer_y)bar_height=visulizer_y;
//    rect(i+width/2, height - fft.getBand(i)*8, 10, height);
//    rect(i+visulizer_x+y, visulizer_y+visulizer_height - fft.getBand(i)*8, i+visulizer_x+y+5,visulizer_height+visulizer_y);
//    rect(i+visulizer_x+y, visulizer_y+visulizer_height - fft.getFreq(i)*4, i+visulizer_x+y+5-10,visulizer_height+visulizer_y);
//    rect(i+visulizer_x+y, visulizer_y+visulizer_height - fft.getBand(i)*4, i+visulizer_x+y+5-10,visulizer_height+visulizer_y);
    rect(/*i+*/y+visulizer_x, /*visulizer_y+visulizer_height - fft.getBand(i)*gain*/bar_height,/*i+*/y+visulizer_x+bar_width,visulizer_height+visulizer_y);
//    rect(i+visulizer_x+y, (visulizer_y+visulizer_height) - (fft.getBand(i)*8), 10, visulizer_height);
    y=y+bar_spacing+bar_width;
    if((y+visulizer_x)>(visulizer_width+visulizer_x))break;
  }
rectMode(CORNER);  
noFill();  
switch (state)
  {
    case "PAUSE" :
    text("PAUSED", visulizer_x+(visulizer_width/2)-(textWidth("PAUSED")/2),visulizer_y+(visulizer_height/2));
    break;
    case "STOP" :
    text("STOPPED", visulizer_x+(visulizer_width/2)-(textWidth("PAUSED")/2),visulizer_y+(visulizer_height/2));
    break;
  }
}
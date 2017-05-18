PImage img_play;
PImage img_pause;
PImage img_stop;
PImage img_next;
PImage img_previous;
PImage img_background;
PImage img_volume;
PImage img_play_pause;
PImage img_ball;
int play_pause_x;
int play_pause_y;
int stop_x;
int stop_y;
int next_x;
int next_y;
int previous_x;
int previous_y;
int volume_x;
int volume_y;
int ball_x;
int ball_y;
int slider_x;
int slider_y;
int control_width=80;
int control_height=80;
int ball_width=32;
int ball_height=32;
int vertical_padding=20;
int horizontal_padding=20;
int slider_height=5;
int slider_width=width-(horizontal_padding*2);

void load_images()
{
img_play=loadImage("/home/pi/Downloads/play_pause_icons/play.png");
img_pause=loadImage("/home/pi/Downloads/play_pause_icons/pause.png");
img_play_pause=loadImage("/home/pi/Downloads/play_pause_icons/play_pause.png");
img_stop=loadImage("/home/pi/Downloads/play_pause_icons/stop.png");
img_next=loadImage("/home/pi/Downloads/play_pause_icons/next.png");
img_previous=loadImage("/home/pi/Downloads/play_pause_icons/previous.png");
img_volume=loadImage("/home/pi/Downloads/play_pause_icons/volume.png");
img_ball=loadImage("/home/pi/Downloads/play_pause_icons/ball.png");
img_background=loadImage("/home/pi/Downloads/play_pause_icons/background_1024x768.jpg");
println("LoadImages......Done!");
}

void draw_controls()
{
int total_horizontal_controls=3;
int spacing = (width-((control_width*total_horizontal_controls)/*+(padding*2)*/))/(total_horizontal_controls+1);
//int slider_height=5;
//int slider_width=width-(horizontal_padding*2);

image(img_background,0,0,width,height);

image(img_previous,0+10,(height/2)-(control_height/2),control_width,control_height);
previous_x=0+10;
previous_y=(height/2)-(control_height/2);

image(img_next,width-control_width-10,(height/2)-(control_height/2),control_width,control_height);
next_x=width-control_width-10;
next_y=(height/2)-(control_height/2);

//image(img_play,padding,height-padding-control_height,control_width,control_height);
image(img_play_pause,spacing,height-vertical_padding-control_height,control_width,control_height);
play_pause_x=spacing;
play_pause_y=height-vertical_padding-control_height;
//image(img_pause,padding+(control_width*1)+(spacing*1),height-padding-control_height,control_width,control_height);

image(img_stop,spacing+(control_width*1)+(spacing*1),height-vertical_padding-control_height,control_width,control_height);
stop_x=spacing+(control_width*1)+(spacing*1);
stop_y=height-vertical_padding-control_height;

image(img_volume,spacing+(control_width*2)+(spacing*2),height-vertical_padding-control_height,control_width,control_height);
volume_x=spacing+(control_width*2)+(spacing*2);
volume_y=height-vertical_padding-control_height;

fill(0,255,0);
rect(horizontal_padding,height-vertical_padding-control_height-50,slider_width,slider_height);//slider
noFill();
slider_x=horizontal_padding;
slider_y=height-vertical_padding-control_height-50;

//image(img_ball,horizontal_padding-(ball_width/2),height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2),ball_width,ball_height);//slider ball
ball_x=horizontal_padding-(ball_width/2);
ball_y=height-vertical_padding-control_height-50-(ball_height/2)+(slider_height/2);
//println("Draw Controls......Done!");
}
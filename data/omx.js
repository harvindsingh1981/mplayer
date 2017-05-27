
var omxp = require('omxplayer-controll');
var WebSocket = require('ws');
var ws = new WebSocket("ws://localhost:8025/omx");

var win_pos=process.argv.slice(2);
console.log(win_pos);
var opts = {
    'audioOutput': 'local', //  'hdmi' | 'local' | 'both'
    'blackBackground': false, //false | true | default: true
    'disableKeys': true, //false | true | default: false
    'disableOnScreenDisplay': true, //false | true | default: false
    'disableGhostbox': true, //false | true | default: false
    'subtitlePath': '', //default: ""
    'startAt': 0, //default: 0
    'startVolume': 0.8, //0.0 ... 1.0 default: 1.0
    'alpha' : 200,
    'win' : win_pos //'0 0 500 500'
};
console.log("omx.js.....Ready!!!")
//omxp.open('/home/pi/Downloads/Sintel_DivXPlus_6500kbps.mkv', opts);

omxp.on('changeStatus',function(status){
    console.log('Status',status);
});
omxp.on('aboutToFinish',function(){
    console.log('File about to finish');
});

//*********WebSocket Implementation**************************

ws.onopen = function()
{
  // Web Socket is connected, send data using send()
     ws.send("[omx.js]Hello Player.....omx.js Here !!!");
     console.log("Websocket opened Sucessfully...");
};

var command=0;
var received_command="";
ws.onmessage = function (evt) 
{ 
  var received_msg = evt.data;
  if(command==0)
	{
		received_command=received_msg;
		ws.send("[omx.js]Command Received...." + received_command);
		command=1;
	}
  else
	{
		ws.send("[omx.js]parameter Received...." + received_msg);
		command=0;
	}
  if(command==0 && received_command !="")
	{
		ws.send("[omx.js]Execiting Command...."+received_command);
		switch(received_command)
		{
			case"START":
			omxp.open(received_msg, opts);
			break;
			case"PLAY":
			break;
			case"PAUSE":
			break;
		}
		ws.send("[omx.js]"+received_command+"....Command Executed");
	}
};
				
ws.onclose = function()
{ 
  // websocket is closed.
  console.log("Connection is closed..."); 
};

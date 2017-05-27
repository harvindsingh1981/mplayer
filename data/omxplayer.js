var omxp = require('omxplayer-controll');
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
    'win' : '0 0 500 500'
};
omxp.open('/home/pi/Downloads/Sintel_DivXPlus_6500kbps.mkv', opts);


omxp.on('changeStatus',function(status){
    console.log('Status',status);
});
omxp.on('aboutToFinish',function(){
    console.log('File about to finish');
});
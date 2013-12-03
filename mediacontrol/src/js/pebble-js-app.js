Pebble.addEventListener('showConfiguration', function() {
    var ipAddress = localStorage.getItem('ipAddress');
    var url = 'http://zernyu.github.io/pebble-mediacontrol/' + 
        (ipAddress !== '' ? '#' + ipAddress : '')
    Pebble.openURL(url);
    console.log('Opening configuration window at URL: ' + url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    console.log('Configuration window returned: ' + e.response);
    if (typeof e.response === 'string') {
        localStorage.setItem('ipAddress', e.response);
    }
});

function sendRequest(mediaRequest) {
    var ipAddress = localStorage.getItem('ipAddress');

    if (ipAddress) {
        var request = 'http://' + ipAddress + '/' + mediaRequest;
        console.log('Sending request: ' + request);

        var req = new XMLHttpRequest();
        req.open('GET', request, true);
        req.send(null);
    } else {
        console.log('No IP address set');
    }
}

Pebble.addEventListener('appmessage', function(e) {
    console.log('received command: ' + e.payload.request);

    sendRequest(e.payload.request);
});

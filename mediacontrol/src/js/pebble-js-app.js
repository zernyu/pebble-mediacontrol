/*
    The JS in this app handles both configuring the IP address to send
    requests to as well as sending the requests themselves.
 */

/*
    Simple configuration stuff
*/
Pebble.addEventListener('showConfiguration', function() {
    // If there's already an IP address in localStorage, prepopulate the
    // config page with it through the URL hash
    var ipAddress = localStorage.getItem('ipAddress');
    var url = 'http://zernyu.github.io/pebble-mediacontrol/' + 
        (ipAddress !== '' ? '#' + ipAddress : '');
    Pebble.openURL(url);
    console.log('Opening configuration window at URL: ' + url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    console.log('Configuration window returned: ' + e.response);
    // There is some JS form validation on the configuration page itself...
    // It shooooould be good enough to trust?
    if (typeof e.response === 'string') {
        var ipAddress = e.response;
        localStorage.setItem('ipAddress', ipAddress);

        Pebble.sendAppMessage({
            "ip_address": (ipAddress ? ipAddress : "")
        });
    }
});

/*
    Simple request stuff
*/
function sendRequest(e) {
    var ipAddress = localStorage.getItem('ipAddress');

    if (ipAddress) {
        var mediaRequest = e.payload.request;
        var request = 'http://' + ipAddress + '/' + mediaRequest;
        console.log('Sending request: ' + request);

        var req = new XMLHttpRequest();
        req.open('GET', request, true);
        req.send(null);
    } else {
        console.log('No IP address set');
    }
}

Pebble.addEventListener('appmessage', sendRequest);

Pebble.addEventListener('ready', function(e) {
    // Tell the Pebble what IP address we'll be sending requests to
    var ipAddress = localStorage.getItem('ipAddress');
    Pebble.sendAppMessage({
        "ip_address": (ipAddress ? ipAddress : "")
    });
});

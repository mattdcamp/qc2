var LISTEN_HOST = '192.168.1.108';
var LISTEN_PORT = 4242;

(function (window, net, console, undefined) {
    'use strict';

    var server = net.createServer();
    server.listen(LISTEN_PORT, LISTEN_HOST);
    console.log("Server listing on: " + LISTEN_HOST + ":" + LISTEN_PORT);
    server.on('connection', function (sock) {
        console.log('CONNECTED: ' + sock.remoteAddress + ':' + sock.remotePort);
        sock.on('data', function (data) {
            var str, arr, type;
            if (data && data.toString()) {
                str = data.toString();
                arr = str.split(',');
                type = arr[0];

                console.log(str);

                switch (type) {
                case 'attitude':
                    window.attitudeCollection.add({
                        pitch: parseFloat(arr[1]),
                        roll: parseFloat(arr[2]),
                        heading: parseFloat(arr[3]),
                        time: parseInt(arr[4])
                    });
                    break;
                case 'gps':
                    window.gpsCollection.add({
                        lat: parseFloat(arr[0]),
                        lng: parseFloat(arr[1]),
                        altitude: parseFloat(arr[3]),
                        time: parseInt(arr[4])
                    });
                    break;
                default:
                    console.log('Unknown message: ' + str);
                }
            }
        });
        sock.on('close', function (data) {
            console.log('connection lost. Listing for reconnect');
            server.listen(LISTEN_PORT, LISTEN_HOST);
        });
    });
})(window, require('net'), window.console);
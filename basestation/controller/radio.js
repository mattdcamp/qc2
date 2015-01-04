var LISTEN_HOST = '192.168.1.108';
var LISTEN_PORT = 4242;

(function (window, net, console, undefined) {
    'use strict';

    var server = net.createServer();
    server.listen(LISTEN_PORT, LISTEN_HOST);
    console.log("Server listing on: " + LISTEN_HOST + ":" + LISTEN_PORT);
    server.on('connection', function (sock) {
        console.log('CONNECTED: ' + sock.remoteAddress + ':' + sock.remotePort);
        
        var handleChange = function() {
            var msg = window.commandModel.toCsv();
            console.log('Sending: ' + msg);
            sock.write(msg);
        };
        window.commandModel.on('change', handleChange);
        
        sock.on('data', function (data) {
            var str, arr, type;
            if (data && data.toString()) {
                str = data.toString();
                arr = str.split(',');
                type = arr[0];

                console.log('Receiving: ' + str);

                switch (type) {
                case 'attitude':
                    window.attitudeCollection.add({
                        pitch: parseFloat(arr[1]),
                        roll: parseFloat(arr[2]),
                        heading: parseFloat(arr[3]),
                        height: parseFloat(arr[4]),
                        time: parseInt(arr[5])
                    });
                    break;
                case 'gps':
                    window.gpsCollection.add({
                        lat: parseFloat(arr[1]),
                        lng: parseFloat(arr[2]),
                        altitude: parseFloat(arr[3]),
                        time: parseInt(arr[4])
                    });
                    break;
                case 'cmd':
                    window.commandModelResponse.set({
                        pitch: parseFloat(arr[1]),
                        roll: parseFloat(arr[2]),
                        heading: parseFloat(arr[3]),
                        height: parseFloat(arr[4]),
                        thrust: parseFloat(arr[5])
                    })
                    break;
                default:
                    console.log('Unknown message: ' + str);
                }
            }
        });
        
        sock.on('close', function (data) {
            window.commandModel.off('change', handleChange);
            
            console.log('connection lost. Listing for reconnect');
            server.listen(LISTEN_PORT, LISTEN_HOST);
        });
    });
    window.sendCommand = function() {
        console.log('Not Connected. Please establish connection before issueing commands');
    };
})(window, require('net'), window.console);
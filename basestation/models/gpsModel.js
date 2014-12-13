(function (widnow, Backbone, undefined) {
    'use strict';
    
    var GpsModel = Backbone.Model.extend({
        defaults: {
            lat: 0,
            lng: 0,
            altitude: 0,
            time: 0
        }
    }),
        GpsCollection = Backbone.Collection.extend({
            model: GpsModel
        });
    window.gpsCollection = new GpsCollection();
})(window, window.Backbone);
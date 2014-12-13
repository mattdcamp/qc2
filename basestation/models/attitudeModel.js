(function (widnow, Backbone, undefined) {
    'use strict';
    var AttitudeModel = Backbone.Model.extend({
            defaults: {
                pitch: 0,
                roll: 0,
                heading: 0,
                time: 0
            }
        }),
        AttitudeCollection = Backbone.Collection.extend({
            model: AttitudeModel
        });

    window.attitudeCollection = new AttitudeCollection();
})(window, window.Backbone);
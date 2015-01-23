(function (widnow, Backbone, undefined) {
    'use strict';
    
    var MotorModel = Backbone.Model.extend({
        defaults: {
            m1:0,
            m2:0,
            m3:0,
            m4:0
        }
    });
    
    var MotorCollection = Backbone.Collection.extend({
        model: MotorModel
    });
    
    window.motorCollection = new MotorCollection();
})(window, window.Backbone);
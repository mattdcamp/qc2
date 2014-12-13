(function (window, $, Backbone, attitudeCollection, undefined) {
    'use strict';
    
    var CommanderAttitudeView = Backbone.View.extend({
        el: '#attitudeSensor',
        attitudeSensor: undefined,
        collection: undefined,
        initialize: function () {
            this.render();
        
            this.listenTo(this.collection, 'add', this.updateAttitude);
        },
        render: function () {
            this.attitudeSensor = $.flightIndicator(this.el, 'attitude', {roll: 0, pitch: 0, size: 200, showBox: true});
        },
        updateAttitude: function (attitude) {
            this.attitudeSensor.setPitch(attitude.get('pitch'));
            this.attitudeSensor.setRoll(attitude.get('roll'));
        },
        remove: function () {
            this.stopListening();
        }
    });
    
    window.CommanderPageView = Backbone.View.extend({
        el: '#viewContainer',
        attitudeView: undefined,
        initialize: function () {
            var that = this;
            this.render();
        
            this.attitudeView = new CommanderAttitudeView({collection: attitudeCollection});
        },
        render: function () {
            var html = '';
            html += '<div id="attitudeSensor" class="col-md-4 sensorContainer"></div>';
            html += '<div id="headingSensor" class="col-md-4 sensorContainer"></div>';
            html += '<div id="altitidue" class="col-md-4 sensorContainer"></div>';
            this.$el.html(html);
            return this;
        },
        remove: function () {
            this.attitudeView.remove();
            this.$el.empty();
        }
    });
})(window, window.jQuery, window.Backbone, window.attitudeCollection);
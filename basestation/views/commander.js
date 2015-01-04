(function ($, Backbone, attitudeCollection, commandModel, undefined) {
    'use strict';
    
    var CommanderAttitudeView = Backbone.View.extend({
        el: '#attitudeSensor',
        attitudeSensor: undefined,
        collection: undefined,
        initialize: function () {
            this.render();
        
            this.listenTo(this.collection, 'add', this.updateAttitude, this);
        },
        render: function () {
            this.attitudeSensor = $.flightIndicator(this.el, 'attitude', {roll: 0, pitch: 0, size: 200, showBox: true});
            if (this.collection.length) {
                this.updateAttitude(this.collection.last());
            }
        },
        updateAttitude: function (attitude) {
            this.attitudeSensor.setPitch(attitude.get('pitch'));
            this.attitudeSensor.setRoll(attitude.get('roll'));
        },
        remove: function () {
            this.stopListening();
        }
    });
    
    var CommanderHeadingView = Backbone.View.extend({
        el: '#headingSensor',
        headingSensor: undefined,
        collection: undefined,
        initialize: function () {
            this.render();
        
            this.listenTo(this.collection, 'add', this.updateHeading, this);
        },
        render: function () {
            this.headingSensor = $.flightIndicator(this.el, 'heading', {heading: 0, showBox: true});
            if (this.collection.length) {
                this.updateHeading(this.collection.last());
            }
        },
        updateHeading: function (attitude) {
            this.headingSensor.setHeading(attitude.get('heading'));
        },
        remove: function () {
            this.stopListening();
        }
    });
    
    var CommanderAltitudeView = Backbone.View.extend({
        el: '#altitidueSensor',
        altitudeSensor: undefined,
        collection: undefined,
        initialize: function () {
            this.render();
        
            this.listenTo(this.collection, 'add', this.updateAltitude, this);
        },
        render: function () {
            this.altitudeSensor = $.flightIndicator(this.el, 'altimeter', {altitude: 0, showBox: true});
            if (this.collection.length) {
                this.updateAltitude(this.collection.last());
            }
        },
        updateAltitude: function (attitude) {
            this.altitudeSensor.setAltitude(attitude.get('altitude'));
        },
        remove: function () {
            this.stopListening();
        }
    });
    
    window.CommanderPageView = Backbone.View.extend({
        el: '#viewContainer',
        attitudeView: undefined,
        headingView: undefined,
        altitudeView: undefined,
        events: {
            'click #sendCmd': "sendCmd"
        },
        initialize: function () {
            var that = this;
            this.commandModel = commandModel;
            this.render();
        
            this.attitudeView = new CommanderAttitudeView({collection: attitudeCollection});
            this.headingView = new CommanderHeadingView({collection: attitudeCollection});
            this.altitudeView = new CommanderAltitudeView({collection: attitudeCollection});
        },
        render: function () {
            var html = window.Handlebars.templates.commander({});
            this.$el.html(html);
            
            $('.nav.navbar-nav li').removeClass('active');
            $('#commander').addClass('active');
            
            return this;
        },
        sendCmd: function(e) {
            this.commandModel.set({
                pitch: this.$el.find('#pitch').val(),
                roll: this.$el.find('#roll').val(),
                heading: this.$el.find('#heading').val(),
                height: this.$el.find('#height').val(),
                thrust: this.$el.find('#thrust').val()
            });
        },
        remove: function () {
            this.attitudeView.remove();
            this.$el.empty();
        }
    });
})(window.jQuery, window.Backbone, window.attitudeCollection, window.commandModel);
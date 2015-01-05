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
            
            this.listenTo(this.commandModel, 'change', this.updatePlaceholder, this);
        },
        render: function () {
            var html = window.Handlebars.templates.commander(this.commandModel.attributes);
            this.$el.html(html);
            
            $('.nav.navbar-nav li').removeClass('active');
            $('#commander').addClass('active');
            
            this.$pitch = this.$el.find('#pitch');
            this.$roll = this.$el.find('#roll');
            this.$heading = this.$el.find('#heading');
            this.$height = this.$el.find('#height');
            this.$thrust = this.$el.find('#thrust');
            
            return this;
        },
        updatePlaceholder: function(command) {
            this.$pitch.attr('placeholder', command.get('pitch'));
            this.$roll.attr('placeholder', command.get('roll'));
            this.$heading.attr('placeholder', command.get('heading'));
            this.$height.attr('placeholder', command.get('height'));
            this.$thrust.attr('placeholder', command.get('thrust'));
            
            this.$el.find('input').val('');
        },
        sendCmd: function(e) {
            var set = {},
                pitch = this.$pitch.val(),
                roll = this.$roll.val(),
                heading = this.$heading.val(),
                height = this.$height.val(),
                thrust = this.$thrust.val();
            
            if(pitch) {set.pitch = pitch;}
            if(roll) {set.roll = roll;}
            if(heading) {set.heading = heading;}
            if(height) {set.height = height;}
            if(thrust) {set.thrust = thrust;}
            
            this.commandModel.set(set);
        },
        remove: function () {
            this.attitudeView.remove();
            this.$el.empty();
        }
    });
})(window.jQuery, window.Backbone, window.attitudeCollection, window.commandModel);
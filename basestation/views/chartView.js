(function (window, $, Backbone, Highcharts, attitudeCollection, commandModelResponse, undefined) {
    'use strict';

    var AttitudeChart = Backbone.View.extend({
        el: '#attitudeChart',
        initialize: function () {
            this.collection = attitudeCollection;
            this.commandModel = commandModelResponse;
            
            this.render();
            
            this.listenTo(this.collection, 'add', this.addPointSingle, this);
            this.addAllPoints();
            
            this.listenTo(this.commandModel, 'change', this.updateCommand, this);
            this.updateCommand(this.commandModel);
        },
        render: function () {
            this.chart = new Highcharts.Chart({
                chart: {
                    type: 'spline',
                    renderTo: this.$el.attr('id'),
                    animation: Highcharts.svg, // don't animate in old IE
                    marginRight: 0
                },
                title: {
                    text: 'Attitude'
                },
                tooltip: {
                    enabled: false
                },
                xAxis: {
                    title: {
                        text: 'System Seconds'
                    },
                    type: 'linear',
                    min: 0,
                    floor: 0
                },
                yAxis: {
                    title: {
                        text: 'Angle (degrees)'
                    },
                    min: -180,
                    max: 180
                },
                legend: {
                    enabled: true
                },
                exporting: {
                    enabled: false
                },
                series: [{
                    id: 'pitch',
                    name: 'Pitch',
                    color: '#00ff00',
                    data: []
                }, {
                    id: 'roll',
                    name: 'Roll',
                    color: '#0000ff',
                    data: []
                }]
            });

            return this;
        },
        addPoint: function (attitude) {
            this.chart.get('pitch').addPoint([attitude.get('time'), attitude.get('pitch')], false, false);
            this.chart.get('roll').addPoint([attitude.get('time'), attitude.get('roll')], false, false);
        },
        addAllPoints: function () {
            this.collection.each(this.addPoint, this);
            this.chart.redraw();
        },
        addPointSingle: function (attitude) {
            this.addPoint(attitude);
            this.chart.redraw();
        },
        updateCommand: function (command) {
            this.updatePitchCommand(command);
            this.updateRollCommand(command);
        },
        updatePitchCommand: function (command) {
            var id = 'pitchCmd',
                axis = this.chart.yAxis[0],
                plotLine;
            
            axis.removePlotLine(id);
            plotLine = axis.addPlotLine({
                id: id,
                label: 'Pitch Command',
                value: command.get('pitch'),
                width: 3,
                color: '#00ff00'
            });
            plotLine.render();
        },
        updateRollCommand: function (command) {
            var id = 'rollCmd',
                axis = this.chart.yAxis[0],
                plotLine;
            
            axis.removePlotLine(id);
            plotLine = axis.addPlotLine({
                id: id,
                label: 'Roll Command',
                value: command.get('roll'),
                width: 3,
                color: '#0000ff'
            });
            plotLine.render();
        },
        remove: function () {
            this.chart.destroy();
            this.stopListening();
        }
    });
    
    var HeightChart = Backbone.View.extend({
        el: '#heightChart',
        initialize: function () {
            this.collection = attitudeCollection;
            this.commandModel = commandModelResponse;
            
            this.render();
            
            this.listenTo(this.collection, 'add', this.addPointSingle, this);
            this.addAllPoints();
            
            this.listenTo(this.commandModel, 'change', this.updateCommand, this);
            this.updateCommand(this.commandModel);
        },
        render: function () {
            this.chart = new Highcharts.Chart({
                chart: {
                    type: 'spline',
                    renderTo: this.$el.attr('id'),
                    animation: Highcharts.svg, // don't animate in old IE
                    marginRight: 0
                },
                title: {
                    text: 'Height'
                },
                tooltip: {
                    enabled: false
                },
                xAxis: {
                    title: {
                        text: 'System Seconds'
                    },
                    type: 'linear',
                    min: 0,
                    floor: 0
                },
                yAxis: {
                    title: {
                        text: 'Height (cm)'
                    },
                    min: 0,
                    max: 800
                },
                legend: {
                    enabled: true
                },
                exporting: {
                    enabled: false
                },
                series: [{
                    id: 'height',
                    name: 'Height',
                    data: [],
                    color: '#ff0000'
                }]
            });
        },
        addPoint: function (attitude) {
            this.chart.get('height').addPoint([attitude.get('time'), attitude.get('height')], false, false);
        },
        addPointSingle: function (attitude) {
            this.addPoint(attitude);
            this.chart.redraw();
        },
        addAllPoints: function () {
            this.collection.each(this.addPoint, this);
            this.chart.redraw();
        },
        updateCommand: function (command) {
            var id = 'heightCmd',
                cmdHeight = command.get('height'),
                axis = this.chart.yAxis[0],
                plotLine;
            
            axis.removePlotLine(id);
            plotLine = axis.addPlotLine({
                id: id,
                label: 'Height Command',
                value: cmdHeight,
                width: 3,
                color: '#ff0000'
            });
            plotLine.render();
        },
        remove: function () {
            this.chart.destroy();
            this.stopListening();
        }
    });
    
    window.ChartPageView = Backbone.View.extend({
        el: '#viewContainer',
        initialize: function () {
            var that = this;
            this.render();
            this.attitudeChart = new AttitudeChart();
            this.heightChart = new HeightChart();
        },
        render: function () {
            this.$el.html('<div id="attitudeChart" class="col-md-12 graphContainer"></div><div id="heightChart" class="col-md-12 graphContainer"></div>');
            
            $('.nav.navbar-nav li').removeClass('active');
            $('#charts').addClass('active');
            return this;
        },
        remove: function () {
            this.attitudeChart.remove();
            this.heightChart.remove();
            this.$el.empty();
        }
    });

})(window, window.jQuery, window.Backbone, window.Highcharts, window.attitudeCollection, window.commandModelResponse);
(function (window, $, Backbone, Highcharts, attitudeCollection, commandModelResponse, motorCollection, undefined) {
    'use strict';

    var ChartView = Backbone.View.extend({
        initialize: function () {
            this.setCollection();
            this.render();
            this.setupBindings();
        },
        render: function () {
            var self = this;
            this.chart = new Highcharts.Chart({
                chart: {
                    type: 'spline',
                    renderTo: this.$el.attr('id'),
                    animation: Highcharts.svg, // don't animate in old IE
                    marginRight: 0
                },
                title: {
                    text: self.chartTitle
                },
                tooltip: {
                    enabled: false
                },
                xAxis: {
                    title: {
                        text: 'Time'
                    },
                    type: 'datetime'
                },
                yAxis: {
                    title: {
                        text: self.axisTitle
                    },
                    min: self.axisMin,
                    max: self.axisMax
                },
                legend: {
                    enabled: true
                },
                exporting: {
                    enabled: false
                },
                series: self.getSeries()
            });

            return this;
        },
        remove: function () {
            this.chart.destroy();
            this.stopListening();
        }
    });
    
    var AttitudeChart = ChartView.extend({
        el: '#attitudeChart',
        setCollection: function () {
            this.collection = attitudeCollection;
            this.commandModel = commandModelResponse;
        },
        setupBindings: function () {
            this.listenTo(this.collection, 'add', this.addPointSingle, this);
            this.addAllPoints();
            
            this.listenTo(this.commandModel, 'change', this.updateCommand, this);
            this.updateCommand(this.commandModel);
        },
        getChartTitle: 'Attitude',
        getAxisTitle: 'Angle (degrees)',
        getAxisMin: -180,
        getAxisMax: 180,
        getSeries: function () {
            return [{
                    id: 'pitch',
                    name: 'Pitch',
                    color: '#00ff00',
                    data: []
                }, {
                    id: 'roll',
                    name: 'Roll',
                    color: '#0000ff',
                    data: []
                }];
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
        }
    });
    
    var HeightChart = ChartView.extend({
        el: '#heightChart',
        setCollection: function() {
            this.collection = attitudeCollection;
            this.commandModel = commandModelResponse;
        },
        setupBindings: function() {
            this.listenTo(this.collection, 'add', this.addPointSingle, this);
            this.addAllPoints();
            
            this.listenTo(this.commandModel, 'change', this.updateCommand, this);
            this.updateCommand(this.commandModel);
        },
        getChartTitle: 'Height',
        getAxisTitle: 'Height (cm)',
        getAxisMin: 0,
        getAxisMax: 800,
        getSeries: function() {
            return [{
                    id: 'height',
                    name: 'Height',
                    data: [],
                    color: '#ff0000'
                }];
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
        }
    });
    
    var MotorChart = ChartView.extend({
        el: '#motorChart',
        setCollection: function () {
            this.collection = motorCollection;
        },
        setupBindings: function () {
            this.listenTo(this.collection, 'add', this.addPointSingle, this);
            this.addAllPoints();
        },
        getChartTitle: 'Motor Control',
        getAxisTitle: 'PWM Signal (us)',
        getAxisMin: -1000,
        getAxisMax: 1000,
        getSeries: function () {
            return [{
                    id: 'm1',
                    name: 'Motor 1',
                    data: []
                }, {
                    id: 'm2',
                    name: 'Motor 2',
                    data: []
                }, {
                    id: 'm3',
                    name: 'Motor 3',
                    data: []
                }, {
                    id: 'm4',
                    name: 'Motor 4',
                    data: []
                }];
        },
        addPoint: function (motor) {
            this.chart.get('m1').addPoint([motor.get('time'), motor.get('m1')], false, false);
            this.chart.get('m2').addPoint([motor.get('time'), motor.get('m2')], false, false);
            this.chart.get('m3').addPoint([motor.get('time'), motor.get('m3')], false, false);
            this.chart.get('m4').addPoint([motor.get('time'), motor.get('m4')], false, false);
        },
        addAllPoints: function () {
            this.collection.each(this.addPoint, this);
            this.chart.redraw();
        },
        addPointSingle: function (motor) {
            this.addPoint(motor);
            this.chart.redraw();
        }
    });
    
    
    window.ChartPageView = Backbone.View.extend({
        el: '#viewContainer',
        initialize: function () {
            var that = this;
            this.render();
            this.attitudeChart = new AttitudeChart();
            this.heightChart = new HeightChart();
            this.motorChart = new MotorChart();
        },
        render: function () {
            this.$el.html('<div id="attitudeChart" class="col-md-6 graphContainer"></div><div id="heightChart" class="col-md-6 graphContainer"></div><div id="motorChart" class="col-md-12 graphContainer"></div>');
            
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

})(window, window.jQuery, window.Backbone, window.Highcharts, window.attitudeCollection, window.commandModelResponse, window.motorCollection);
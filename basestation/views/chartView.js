(function (window, $, Backbone, Highcharts, attitudeCollection, undefined) {
    'use strict';

    var AttitudeChart = Backbone.View.extend({
        el: '#attitudeChart',
        initialize: function () {
            this.render();
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
                    data: []
                }, {
                    id: 'roll',
                    name: 'Roll',
                    data: []
                }]
            });

            this.listenTo(this.collection, 'add', this.addPointSingle);
            this.addAllPoints();

            return this;
        },
        addPoint: function (attitude) {
            this.chart.get('pitch').addPoint([attitude.get('time'), attitude.get('pitch')], false, false);
            this.chart.get('roll').addPoint([attitude.get('time'), attitude.get('roll')], false, false);
        },
        addAllPoints: function (attitudes) {
            this.collection.each(this.addPoint);
            this.chart.redraw();
        },
        addPointSingle: function (attitude) {
            this.addPoint(attitude);
            this.chart.redraw();
        },
        remove: function () {
            this.chart.destroy();
            this.stopListening();
        }
    });

    window.ChartPageView = Backbone.View.extend({
        el: '#viewContainer',
        attitudeChart: undefined,
        initialize: function () {
            var that = this;
            this.render();
            this.attitudeChart = new AttitudeChart({
                collection: attitudeCollection
            });
        },
        render: function () {
            this.$el.html('<div id="attitudeChart" class="col-md-12 graphContainer"></div>');
            return this;
        },
        remove: function () {
            this.attitudeChart.remove();
            this.$el.empty();
        }
    });

})(window, window.jQuery, window.Backbone, window.Highcharts, window.attitudeCollection);
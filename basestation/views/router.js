(function (window, $, Backbone, ChartPageView, CommanderPageView) {
    'use strict';
    var Router = Backbone.Router.extend({
            routes: {
                "": "chart",
                "index": "home",
                "chart": "chart",
                "commander": "commander"
            },
            home: function () {
                console.log('not implemented');
            },
            chart: function () {
                this.loadView(ChartPageView);
            },
            commander: function () {
                this.loadView(CommanderPageView);
            },
            loadView: function (NewView) {
                if (this.view) {
                    this.view.remove();
                }
                this.view = new NewView();
            }
        }),
        router = new Router();

    $(document).ready(function () {
        Backbone.history.start();
    });
})(window, window.jQuery, window.Backbone, window.ChartPageView, window.CommanderPageView);
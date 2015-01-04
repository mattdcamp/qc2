(function (window, $, Backbone, undefined) {
    'use strict';
    var CommandModel = Backbone.Model.extend({
            defaults: {
                pitch: 0,
                roll: 0,
                heading: 0,
                height: 0,
                thrust: 0
            },
            toCsv: function() {
                var output = "cmd,";
                output += this.get('pitch') + ',';
                output += this.get('roll') + ',';
                output += this.get('heading') + ',';
                output += this.get('height') + ',';
                output += this.get('thrust');
                return output;
            }
        });

    window.commandModel = new CommandModel();
    window.commandModelResponse = new CommandModel();
})(window, window.jQuery, window.Backbone);
$(window).on('load', function() {
    $('.close_container').click(function () {
        var id = $(this).parent('.modalDialog').attr('id');
        var table_id = '#' + id.split('-')[0] + '-table';
        var remove_tr = table_id + ' tr:not(:first)';
        $(remove_tr).remove();

        if(id == 'alarm-dialog')
            checkAlarm();
    });

    $('.detect-tile').click(function() {
        initDetectButton();
        $('#detect-all').addClass('bg-cyan');
        getDetectLog();
    });

    $('.control-tile').click(function() {
        initControlButton();
        $('#control-all').addClass('bg-cyan');
        getControlLog();
    });
});
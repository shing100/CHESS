var obj;

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

    $('.info-tile').click(function() {
        var oriPwd = prompt('기존 비멀번호를 입력하세요.');
        var chgPwd = prompt('변경하실 비밀번호를 입력하세요.');

        $.ajax({
            url: server + '/user/change.do',
            type: 'POST',
            datType: 'json',
            data: {
                'oriPwd' : oriPwd,
                'chgPwd' : chgPwd
            },
            success: function(json) {
                obj = json;

                console.log(json);
                var result = json["result"];
                var message = json["message"];

                if(result == 'success') {
                    var url = server + '/user/logout.do';

                    alert(message);
                    alert('다시 로그인해주세요.');
                    $( location ).attr("href", url);
                } else {
                    alert(message);
                }
            },
            error: function() {
                alert('An error has occurred.\nPlease check your SQL Server.');
            }
        });
    });
});
$('.detect-item').click(function() {
    var id = $(this).attr('id');
    var deviceName = id.split('-')[1];

    if(deviceName == 'all')
        getDetectLog();
    else
        getDetectLogByDeviceName(deviceName);

    clickDetectButton(this);
});

function clickDetectButton(name) {
    initDetectButton();
    $(name).addClass('bg-cyan');
}

function initDetectButton() {
    $('#detect-all').removeClass('bg-cyan');
    $('#detect-firesensor').removeClass('bg-cyan');
    $('#detect-intrusionsensor').removeClass('bg-cyan');
}

$('.control-item').click(function() {
    var id = $(this).attr('id');
    var deviceName = id.split('-')[1];

    if(deviceName == 'all')
        getControlLog();
    else
        getControlLogByDeviceName(deviceName);

    clickControlButton(this);
});

function clickControlButton(name) {
    initControlButton();
    $(name).addClass('bg-cyan');
}

function initControlButton() {
    $('#control-all').removeClass('bg-cyan');
    $('#control-light').removeClass('bg-cyan');
    $('#control-valve').removeClass('bg-cyan');
    $('#control-doorlock').removeClass('bg-cyan');
    $('#control-intrusionsensor').removeClass('bg-cyan');
}

function getAlarm() {
    var remove_tr = '#alarm-table tr:not(:first)';

    $.ajax({
        url: server + '/log/alarm',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var count = json.length;

            $('#alarm-count').text(count);
            $('.alarm-tile').addClass('bg-amber');

            $(remove_tr).remove();

            for(var i=0; i<count; i++)
                appendAlarmState(i, json[i]);
        },
        error: function() {
            $('#alarm-count').text('0');
            $(remove_tr).remove();
            $('.alarm-tile').removeClass('bg-amber');
            $('.alarm-tile').addClass('disable');

            $('#alarm-link').click(function() {
                return false;
            });
        }
    });
}

function appendAlarmState(number, object) {
    var tableName = '#alarm-table';
    var device = object["device"]["name"];
    var date = new Date(object["date"]).format("yy년 MM월 dd일 a/p hh시 mm분 ss초");
    var state = '탐지';
    var dName;

    switch(device) {
        case 'firesensor':
            dName = '화재감지';
            break;
        case 'intrusionsensor':
            dName = '침입감지';
            break;
    }

    var content = '<tr class="log-table-content">' +
        '<td>' + (number+1) + '</td>' +
        '<td>' + date + '</td>' +
        '<td>' + dName + '</td>' +
        '<td>' + state + '</td>' +
        '</tr>'

    $(tableName).append(content);
}

function checkAlarm() {
    $.ajax({
        url: server + '/log/alarm',
        type: 'POST',
        dataType: 'json',
        success: function() {
            $('#alarm-count').text('0');
        }
    });
}

function getDetectLog() {
    removeDetectLog();

    $.ajax({
        url: server + '/log/detect',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var count = json.length;

            for(var i=0; i<count; i++) {
                appendDetectLog(i, json[i]);
            }
        },
        error: function() {
            alert('SQL Server Error');
        }
    });
}

function getDetectLogByDeviceName(deviceName) {
    removeDetectLog();

    $.ajax({
        url: server + '/log/detect/' + deviceName,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var count = json.length;

            for(var i=0; i<count; i++) {
                appendDetectLog(i, json[i]);
            }
        },
        error: function() {
            alert('SQL Server Error');
        }
    });
}

function appendDetectLog(number, object) {
    var tableName = '#detect-table';
    var device = object["device"]["name"];
    var date = new Date(object["date"]).format("yy년 MM월 dd일 a/p hh시 mm분 ss초");
    var dName;

    switch(device) {
        case 'firesensor':
            dName = '화재감지';
            break;
        case 'intrusionsensor':
            dName = '침입감지';
            break;
    }

    var content = '<tr class="log-table-content">' +
        '<td>' + (number+1) + '</td>' +
        '<td>' + date + '</td>' +
        '<td>' + dName + '</td>' +
        '</tr>'

    $(tableName).append(content);
}

function removeDetectLog() {
    var table = '#detect-table  tr:not(:first)';

    $(table).remove();
}

function getControlLog() {
    removeControlLog();

    $.ajax({
        url: server + '/log/control',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var count = json.length;

            for(var i=0; i<count; i++) {
                appendControlLog(i, json[i]);
            }
        },
        error: function() {
            alert('SQL Server Error');
        }
    });
}

function getControlLogByDeviceName(deviceName) {
    removeControlLog();

    $.ajax({
        url: server + '/log/control/' + deviceName,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var count = json.length;

            for(var i=0; i<count; i++) {
                appendControlLog(i, json[i]);
            }
        },
        error: function() {
            alert('SQL Server Error');
        }
    });
}

function appendControlLog(number, object) {
    var tableName = '#control-table';
    var device = object["device"]["name"];
    var date = new Date(object["date"]).format("yy년 MM월 dd일 a/p hh시 mm분 ss초");
    var state = (object['command']) ? 'On' : 'Off';
    var dName;

    switch(device) {
        case 'light':
            dName = '조명';
            break;
        case 'valve':
            dName = '밸브';
            break;
        case 'doorlock':
            dName = '도어락';
            break;
        case 'intrusionsensor':
            dName = '외출모드';
            break;
    }

    var content = '<tr class="log-table-content">' +
        '<td>' + (number+1) + '</td>' +
        '<td>' + date + '</td>' +
        '<td>' + dName + '</td>' +
        '<td>' + state + '</td>' +
        '</tr>'

    $(tableName).append(content);
}

function removeControlLog() {
    var table = '#control-table  tr:not(:first)';

    $(table).remove();
}
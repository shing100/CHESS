function getState() {
    var deviceList = ['valve', 'doorlock'];

    for(var i=0; i<deviceList.length; i++)
        getDeviceState(deviceList[i]);

    getLightState();
    getIntrusionsensor();
    getFireSensor();
    getAlarm();
}

function getLightState() {
    var tile = '.light-tile';

    $.ajax({
        url: server + '/status/light',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var roomObj = json["data"]["room"];

            onTile(tile);

            for(var i=1; i<=roomObj.length; i++) {
                var id = '#room' + i;
                var room = roomObj[i-1]["state"];

                if(room)
                    $(id).addClass('bg-yellow');
                else
                    $(id).removeClass('bg-yellow');
            }
        },
        error: function() {
            disableTile(tile);
            $('#light-link').click(function() {
                return false;
            });
        }
    });
}

function getIntrusionsensor() {
    var intrusionsensor = '.intrusionsensor-tile';
    var go_out = '.go-out-tile';

    $.ajax({
        url: server + '/status/intrusionsensor',
        type: 'GET',
        dataType: 'json',
        success: function (json) {
            var state = json["data"]["state"];
            var power = json["data"]["power"];
            var detect = json["data"]["detect"];

            if(power == true && state == true) {
                onTile(intrusionsensor);
                onTile(go_out);

                if(detect != null && detect == true) {
                    var checking = confirm("침입자 감지");

                    if(checking)
                        checkDeviceAlert('intrusionsensor');
                }
            } else {
                offTile(intrusionsensor);
                offTile(go_out);
            }
        },
        error: function () {
            disableTile(intrusionsensor);
            disableTile(go_out);
        }
    });
}

function getFireSensor() {
    var tile = '.firesensor-tile';

    $.ajax({
        url: server + '/status/firesensor',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var state = json["data"]["state"];
            var detect = json["data"]["detect"];

            $(tile).removeClass('disable');

            (state) ? onTile(tile) : offTile(tile);

            if(detect != null && detect == true) {
                var checking = confirm("화재 감지");

                if(checking)
                    checkDeviceAlert('firesensor');
            }
        },
        error: function() {
            disableTile(tile);
        }
    });
}

function getDeviceState(deviceName) {
    var tile = '.' + deviceName + '-tile';

    $.ajax({
        url: server + '/status/' + deviceName,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            var onOff = json["data"]["state"];

            $(tile).removeClass('disable');

            (onOff) ? onTile(tile) : offTile(tile);
        },
        error: function() {
            disableTile(tile);
        }
    });
}

function checkDeviceAlert(deviceName) {
    $.ajax({
        url: server + '/device/' + deviceName + '/check',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            if(json['status'] != 'success')
                alert('check error has occurred');
        },
        error: function () {
            alert('check error has occurred');
        }
    });
}

function onTile(tile) {
    $(tile).removeClass('bg-grayed disable');
}

function offTile(tile) {
    $(tile).addClass('bg-grayed');
}

function disableTile(tile) {
    $(tile).addClass('bg-grayed disable');
}

getState();

stateUpdate = setInterval(function() {
    getState();
}, 3000);

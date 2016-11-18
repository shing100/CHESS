$('.room').click(function() {
    var selector = this;
    var room = $(this).attr('id');
    var command = ($(this).hasClass('bg-yellow')) ? false : true;
    var roomNumber = room.substring(room.length - 1);

    $.ajax({
        url: server + '/control/light/room/' + roomNumber + '/' + command,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            if(json["status"] == 'success')
                $(selector).toggleClass('bg-yellow');
            else{
                var errCode = json["code"];
                var errMsg = json["message"];

                alert('An error has occurred.\nError code: ' + errCode + '\nError Message: ' + errMsg);
            }
        },
        error: function() {
            alert('An error has occurred.\nPlease check your device.');
        }
    });
});

$('.valve-tile').click(function() {
    var selector = this;
    var command = ($(this).hasClass('bg-pink')) ? false : true;

    $.ajax({
        url: server + '/control/valve/' + command,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            if(json["status"] == 'success')
                $(selector).toggleClass('bg-pink');
            else{
                var errCode = json["code"];
                var errMsg = json["message"];

                alert('An error has occurred.\nError code: ' + errCode + '\nError Message: ' + errMsg);
            }
        },
        error: function() {
            alert('An error has occurred.\nPlease check your device.');
        }
    });
});

$('.doorlock-tile').click(function() {
    var selector = this;
    var command = ($(this).hasClass('bg-cyan')) ? false : true;

    $.ajax({
        url: server + '/control/doorlock/' + command,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            if(json["status"] == 'success')
                alert('열림');
            else{
                var errCode = json["code"];
                var errMsg = json["message"];

                alert('An error has occurred.\nError code: ' + errCode + '\nError Message: ' + errMsg);
            }
        },
        error: function() {
            alert('An error has occurred.\nPlease check your device.');
        }
    });
});

$('.go-out-tile').click(function() {
    var selector = this;
    var command = ($(this).hasClass('bg-green')) ? false : true;

    $.ajax({
        url: server + '/control/intrusionsensor/' + command,
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            if(json["status"] == 'success')
                $(selector).toggleClass('bg-green');
            else{
                var errCode = json["code"];
                var errMsg = json["message"];

                alert('An error has occurred.\nError code: ' + errCode + '\nError Message: ' + errMsg);
            }
        },
        error: function() {
            alert('An error has occurred.\nPlease check your device.');
        }
    });

    $.ajax({
        url: server + '/control/valve/false',
        type: 'GET',
        dataType: 'json',
        success: function(json) {
            if(json["status"] != 'success') {
                var errCode = json["code"];
                var errMsg = json["message"];

                alert('An error has occurred.\nError code: ' + errCode + '\nError Message: ' + errMsg);
            }
        },
        error: function() {
            alert('An error has occurred.\nPlease check your device.');
        }
    });
});
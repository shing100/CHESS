package com.dongyang.service;

import com.dongyang.domain.Device;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * Created by HeemangHan on 2016. 10. 1..
 */

@Service
public class DeviceCheckService {

    @Autowired
    private SocketService socketService;

    @Autowired
    private JsonService jsonService;

    public String sensorCheck(Device device) {
        JsonNodeFactory factory = JsonNodeFactory.instance;
        ObjectNode root = null;

        root = jsonService.createCommandJSON(device, "check");

        return socketService.sendRequest(device.getIpAddress(), root.toString());
    }
}

package com.dongyang.service;

import com.dongyang.domain.Device;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * Created by HeemangHan on 2016. 9. 29..
 */

@Service
public class DeviceStatusService {

    @Autowired
    private SocketService socketService;

    @Autowired
    private JsonService jsonService;

    public String getDeviceStatus(Device device) {
        JsonNodeFactory factory = JsonNodeFactory.instance;
        ObjectNode root = null;

        root = jsonService.createCommandJSON(device, "state");

        return socketService.sendRequest(device.getIpAddress(), root.toString());
    }
}

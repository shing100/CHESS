package com.dongyang.service;

import com.dongyang.domain.ControlLog;
import com.dongyang.domain.Device;
import com.dongyang.repository.ControlLogRepository;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * Created by HeemangHan on 2016. 10. 12..
 */

@Service
public class DeviceControlService {

    @Autowired
    private SocketService socketService;

    @Autowired
    private JsonService jsonService;

    @Autowired
    ControlLogRepository controlLogRepository;

    public String deviceControl(Device device, int roomNumber, boolean command) {
        String jsonResult = null;

        JsonNodeFactory factory = JsonNodeFactory.instance;
        ObjectNode root = null;
        ObjectNode child = factory.objectNode();
        String json = null;

        root = jsonService.createCommandJSON(device, "control");

        child.put("roomNumber", roomNumber);
        child.put("command", command);
        root.put("message", child);

        json = root.toString();

        jsonResult = socketService.sendRequest(device.getIpAddress(), json);

        if(checkResult(jsonResult))
            controlLogRepository.save(new ControlLog(device, command));

        return jsonResult;
    }

    public String deviceControl(Device device, boolean command) {
        String jsonResult = null;

        JsonNodeFactory factory = JsonNodeFactory.instance;
        ObjectNode root = null;
        ObjectNode child = factory.objectNode();
        String json = null;

        root = jsonService.createCommandJSON(device, "control");

        child.put("command", command);

        root.put("message", child);

        json = root.toString();

        jsonResult = socketService.sendRequest(device.getIpAddress(), json);

        if(checkResult(jsonResult))
            controlLogRepository.save(new ControlLog(device, command));

        return jsonResult;
    }

    public boolean checkResult(String json) {
        JsonParser parser = new JsonParser();
        JsonObject object = parser.parse(json).getAsJsonObject();

        String result = object.get("status").getAsString();

        if(result.equals("success"))
            return true;
        else
            return false;
    }
}

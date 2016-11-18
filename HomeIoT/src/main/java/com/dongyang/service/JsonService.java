package com.dongyang.service;

import com.dongyang.domain.Device;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import org.springframework.stereotype.Service;

/**
 * Created by HeemangHan on 2016. 9. 29..
 */

@Service
public class JsonService {

    public JsonObject createJSON(String str) {
        JsonParser jsonParser = new JsonParser();
        JsonObject jsonObject = jsonParser.parse(str).getAsJsonObject();

        return jsonObject;
    }

    public ObjectNode createCommandJSON(Device device, String action) {
        JsonNodeFactory factory = JsonNodeFactory.instance;
        ObjectNode root = factory.objectNode();

        root.put("deviceName", device.getName());
        root.put("action", action);

        return root;
    }
}

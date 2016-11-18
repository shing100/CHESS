package com.dongyang.api;

import com.dongyang.domain.ControlLog;
import com.dongyang.domain.DetectLog;
import com.dongyang.domain.Device;
import com.dongyang.exception.ResourceNotFoundException;
import com.dongyang.repository.ControlLogRepository;
import com.dongyang.repository.DetectLogRepository;
import com.dongyang.repository.DeviceRepository;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonObject;
import org.apache.catalina.servlet4preview.http.HttpServletRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;
import java.util.List;

/**
 * Created by HeemangHan on 2016. 10. 12..
 */

@RestController
@RequestMapping("log")
public class LogController {

    @Autowired
    private DeviceRepository deviceRepository;

    @Autowired
    private DetectLogRepository detectLogRepository;

    @Autowired
    private ControlLogRepository controlLogRepository;

    // 탐지된 값 DB에 저장
    @RequestMapping(value = "detect/{deviceName}", method = RequestMethod.PUT)
    public void recordDetect(@PathVariable String deviceName) {
        Device device = deviceRepository.findByName(deviceName);

        if(device == null)
            throw new ResourceNotFoundException();
        else
            detectLogRepository.save(new DetectLog(device, false));
    }

    // 모든 detect log 불러옴
    @RequestMapping(value = "detect", method = RequestMethod.GET)
    public String getDetectLogList() {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String json = null;

        List<DetectLog> detectLogList = null;

        detectLogList = detectLogRepository.findAllByOrderByDateDesc();

        json = gson.toJson(detectLogList);

        return json;
    }

    // deviceName에 해당하는 detect log 불러옴
    @RequestMapping(value = "detect/{deviceName}", method = RequestMethod.GET)
    public String getDetectLogListByDeviceName(@PathVariable String deviceName) {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String json = null;

        Device device = deviceRepository.findByName(deviceName);
        List<DetectLog> detectLogList = null;

        if(device == null)
            throw new ResourceNotFoundException();
        else
             detectLogList = detectLogRepository.findByDeviceIdOrderByDateDesc(device.getId());

        json = gson.toJson(detectLogList);

        return json;
    }

    // 모든 control log 불러옴
    @RequestMapping(value = "control", method = RequestMethod.GET)
    public String getControlLogList() {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String json = null;

        List<ControlLog> controlLogList = null;

        controlLogList = controlLogRepository.findAllByOrderByDateDesc();

        json = gson.toJson(controlLogList);

        return json;
    }

    // deviceName에 해당하는 control log 불러옴
    @RequestMapping(value = "control/{deviceName}", method = RequestMethod.GET)
    public String getControlLogListByDeviceName(@PathVariable String deviceName) {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String json = null;

        Device device = deviceRepository.findByName(deviceName);
        List<ControlLog> controlLogList = null;

        if(device == null)
            throw new ResourceNotFoundException();
        else
            controlLogList = controlLogRepository.findByDeviceIdOrderByDateDesc(device.getId());

        json = gson.toJson(controlLogList);

        return json;
    }

    // 읽지 않은 알람 불러옴
    @RequestMapping(value = "alarm", method = RequestMethod.GET)
    public String getCountDetect() {
        List<DetectLog> detectLogList = detectLogRepository.findByStateOrderByDateDesc(false);

        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String json = gson.toJson(detectLogList);

        return json;
    }

    // alram 읽음 상태로 바꿈
    @RequestMapping(value = "alarm", method = RequestMethod.POST)
    public String setCountCheck() {
        JsonObject object = new JsonObject();

        int count = 0;
        String status;

        count = detectLogRepository.updateByCheckIsTrue();

        status = (count != 0) ? "success" : "false";

        object.addProperty("status", status);

        return object.toString();
    }
}

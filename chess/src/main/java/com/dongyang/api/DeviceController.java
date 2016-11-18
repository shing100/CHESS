package com.dongyang.api;

import com.dongyang.domain.Device;
import com.dongyang.exception.ResourceNotFoundException;
import com.dongyang.repository.DeviceRepository;
import com.dongyang.service.DeviceCheckService;
import org.apache.catalina.servlet4preview.http.HttpServletRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

/**
 * Created by HeemangHan on 2016. 10. 12..
 */

@RestController
@RequestMapping("/device")
public class DeviceController {

    @Autowired
    private DeviceRepository deviceRepository;

    @Autowired
    private DeviceCheckService deviceCheckService;

    @RequestMapping("{deviceId}")
    public Device getDevice(@PathVariable Integer deviceId) {
        Device device = deviceRepository.findOne(deviceId);
        return device;
    }

    @RequestMapping(value = "sync", method = RequestMethod.POST)
    public void updateDevice(HttpServletRequest request) {
        int id = Integer.parseInt(request.getParameter("id"));
        String ipAddress= request.getParameter("ipAddress");

        System.out.println(id);
        System.out.println(ipAddress);

        Device device = deviceRepository.findOne(id);

        device.setIpAddress(ipAddress);

        deviceRepository.save(device);
    }

    @RequestMapping("find/{deviceName}")
    public Device getDevice(@PathVariable String deviceName) {
        Device device = deviceRepository.findByName(deviceName);

        return device;
    }

    @RequestMapping("{deviceName}/check")
    public String checkDevice(@PathVariable String deviceName) {
        Device device = deviceRepository.findByName(deviceName);

        if(device == null)
            throw new ResourceNotFoundException();

        return deviceCheckService.sensorCheck(device);
    }
}

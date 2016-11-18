package com.dongyang.api;

import com.dongyang.domain.Device;
import com.dongyang.exception.ResourceNotFoundException;
import com.dongyang.repository.DeviceRepository;
import com.dongyang.service.DeviceControlService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

/**
 * Created by HeemangHan on 2016. 9. 26..
 */

@RestController
@RequestMapping("control")
public class DeviceControlController {

    @Autowired
    private DeviceRepository deviceRepository;

    @Autowired
    private DeviceControlService deviceControlService;

    @RequestMapping("/light/room/{roomNumber}/{command}")
    private String controlDevice(@PathVariable("roomNumber") int roomNumber, @PathVariable("command") boolean command) {

        if(roomNumber > 4 || roomNumber < 1)
            throw new ResourceNotFoundException();
        else {
            Device device = deviceRepository.findByName("light");
            return deviceControlService.deviceControl(device, roomNumber, command);
        }
    }

    @RequestMapping("{deviceName}/{command}")
    private String controlDevice(@PathVariable String deviceName, @PathVariable boolean command) {
        Device device = deviceRepository.findByName(deviceName);

        if(device == null)
            throw new ResourceNotFoundException();

        return deviceControlService.deviceControl(device, command);
    }
}

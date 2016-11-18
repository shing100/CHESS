package com.dongyang.api;

import com.dongyang.domain.Device;
import com.dongyang.exception.ResourceNotFoundException;
import com.dongyang.repository.DeviceRepository;
import com.dongyang.service.DeviceStatusService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;

/**
 * Created by HeemangHan on 2016. 9. 26..
 */

@RestController
@RequestMapping("status")
public class DeviceStatusController {

    @Autowired
    private DeviceRepository deviceRepository;

    @Autowired
    private DeviceStatusService deviceService;

    @RequestMapping(value = "{deviceName}", method = RequestMethod.GET)
    String getDeviceStatus(@PathVariable String deviceName) {
        Device device = deviceRepository.findByName(deviceName);

        if(device == null)
            throw new ResourceNotFoundException();
        else
            return deviceService.getDeviceStatus(device);
    }
}

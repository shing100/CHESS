package com.dongyang.repository;

import com.dongyang.domain.Device;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

/**
 * Created by HeemangHan on 2016. 9. 19..
 */

public interface DeviceRepository extends JpaRepository<Device, Integer> {
    Device findByName(String deviceName);
}
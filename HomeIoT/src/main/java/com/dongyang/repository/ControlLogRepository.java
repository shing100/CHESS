package com.dongyang.repository;

import com.dongyang.domain.ControlLog;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

/**
 * Created by HeemangHan on 2016. 10. 12..
 */
public interface ControlLogRepository extends JpaRepository<ControlLog, Integer> {

    List<ControlLog> findAllByOrderByDateDesc();
    List<ControlLog> findByDeviceIdOrderByDateDesc(Integer deviceId);
}

package com.dongyang.repository;

import com.dongyang.domain.DetectLog;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

/**
 * Created by HeemangHan on 2016. 10. 12..
 */
public interface DetectLogRepository extends JpaRepository<DetectLog, Integer> {

    List<DetectLog> findAllByOrderByDateDesc();

    List<DetectLog> findByDeviceIdOrderByDateDesc(Integer deviceId);

    List<DetectLog> findByStateOrderByDateDesc(boolean state);

    @Modifying
    @Transactional
    @Query("update DetectLog set state = true where state = false")
    int updateByCheckIsTrue();
}

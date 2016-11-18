package com.dongyang.domain;

import com.fasterxml.jackson.annotation.JsonBackReference;
import lombok.Data;

import javax.persistence.*;
import java.sql.Timestamp;
import java.util.Date;

/**
 * Created by HeemangHan on 2016. 10. 1..
 */

@Entity
@Data
public class DetectLog {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Integer id;

    @ManyToOne
    @JsonBackReference
    private Device device;

    @Column(nullable = false)
    private boolean state;

    @Column(nullable = false, columnDefinition = "TIMESTAMP DEFAULT CURRENT_TIMESTAMP")
    private Timestamp date;

    public DetectLog() {

    }

    public DetectLog(Device device, boolean state) {
        this.device = device;
        this.state = state;
    }
}
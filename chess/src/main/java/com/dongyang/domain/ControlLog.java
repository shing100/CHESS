package com.dongyang.domain;

import com.fasterxml.jackson.annotation.JsonBackReference;
import lombok.Data;

import javax.persistence.*;
import java.sql.Timestamp;
/**
 * Created by HeemangHan on 2016. 10. 1..
 */

@Data
@Entity
public class ControlLog {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Integer id;

    @ManyToOne
    @JsonBackReference
    private Device device;

    @Column(nullable = false)
    private boolean command;

    @Column(nullable = false, columnDefinition = "TIMESTAMP DEFAULT CURRENT_TIMESTAMP")
    private Timestamp date;

    public ControlLog() {

    }

    public ControlLog(Device device, boolean command) {
        this.device = device;
        this.command = command;
    }
}

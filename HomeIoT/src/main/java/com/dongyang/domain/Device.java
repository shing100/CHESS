package com.dongyang.domain;

import lombok.Data;

import javax.persistence.*;
import java.util.List;

/**
 * Created by HeemangHan on 2016. 8. 4..
 */

@Data
@Entity
public class Device {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Integer id;

    @Column(nullable = false)
    private String name;

    @Column(nullable = false)
    private String ipAddress;

    @Column(nullable = false)
    private int port;
//
//    @OneToMany
//    @JoinColumn(name = "device_id", referencedColumnName = "id")
//    private List<ControlLog> controlLogList;
//
//    @OneToMany
//    @JoinColumn(name = "device_id", referencedColumnName = "id")
//    private List<DetectLog> detectLogList;

    public Device() {

    }

    public Device(String name, String ipAddress, int port) {
        this.name = name;
        this.ipAddress = ipAddress;
        this.port = port;
    }
}
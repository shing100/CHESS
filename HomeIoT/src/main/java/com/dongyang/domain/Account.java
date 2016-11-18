package com.dongyang.domain;

import lombok.Data;

import javax.persistence.*;

/**
 * Created by HeemangHan on 2016. 10. 25..
 */

@Entity
@Data
public class Account {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Integer no;

    @Column(nullable = false)
    private String id;

    @Column(nullable = false)
    private String password;

    @Column(nullable = false)
    private String role;

}